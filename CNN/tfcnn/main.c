#include "modules/api.h"
#include "connector.h"

void modify_roi_out(dt_graph_t *graph, dt_module_t *module)
{
    module->connector[2].roi.full_wd = 3 * 1024 * 1024;
    module->connector[2].roi.full_ht = 1;
}

void modify_roi_in(dt_graph_t *graph, dt_module_t *module)
{
    module->connector[0].roi.wd = module->connector[0].roi.full_wd;
    module->connector[0].roi.ht = module->connector[0].roi.full_ht;
    
    module->connector[1].roi.wd = module->connector[1].roi.full_wd;
    module->connector[1].roi.ht = module->connector[1].roi.full_ht;

    if (module->connector[1].roi.wd != 7 * 1024 * 1024) {
        printf("ERROR\n");
    }
}

// Rounds x to the next multiple of 16
int round16(const int x)
{
    return 16 * (x / 16) + (x % 16 != 0 ? 16 : 0);
}

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    const int FEATURES_COUNT[] = {32, 43, 57, 76, 101, 101};

    int index_weights_buffer = 0; // Beginning of the weights of the next convolution

    // Create the convolution layers of the encoder
    int encoder_convolution_layers[6];

    for (int i = 0; i < 6; i++) {
        const int size = 1 << (10 - i);
        const int input_features_count = i == 0 ? 7 : FEATURES_COUNT[i - 1];
        const int output_features_count = FEATURES_COUNT[i];

        dt_roi_t roi_in = (dt_roi_t){ .wd = round16(input_features_count) * size * size * (i >= 1 ? 4 : 1), .ht = 1 };
        dt_roi_t roi_out = (dt_roi_t){ .wd = round16(output_features_count) * size * size, .ht = 1 };

        int pc[] = { index_weights_buffer };

        char* shader;
        if (i == 0) shader = "conv0";
        if (i == 1) shader = "conv1";
        if (i == 2) shader = "conv2";
        if (i == 3) shader = "conv3";
        if (i == 4) shader = "conv4";
        if (i == 5) shader = "conv5";

        encoder_convolution_layers[i] = dt_node_add(
            graph, module, "tfcnn", shader,
            size / 8 * DT_LOCAL_SIZE_X, size / 8 * DT_LOCAL_SIZE_Y, 1,
            sizeof(pc), pc, 3,
            "weights", "read", "ssbo", "f16", &module->connector[0].roi,
            "output", "write", "ssbo", "f16", &roi_out,
            "input", "read", "ssbo", "f16", &roi_in
        );

        index_weights_buffer += 9 * input_features_count * output_features_count + output_features_count;
        dt_connector_copy(graph, module, 0, encoder_convolution_layers[i], 0);
    }

    // Create the convolution layers of the decoder
    int decoder_convolution_layers[5][2];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 2; j++) {
            const int size = 1 << (6 + i);
            const int input_features_count = j == 0 ? (FEATURES_COUNT[5 - i] + FEATURES_COUNT[4 - i]) : FEATURES_COUNT[4 - i];
            const int output_features_count = (i == 4 && j == 1) ? 3 : FEATURES_COUNT[4 - i];

            dt_roi_t roi_out = (dt_roi_t){ .wd = round16(output_features_count) * size * size, .ht = 1 };

            if (j == 0) {
                char* shader = NULL;
                if (i == 0) shader = "conv6";
                if (i == 1) shader = "conv8";
                if (i == 2) shader = "conv10";
                if (i == 3) shader = "conv12";
                if (i == 4) shader = "conv14";

                int pc[] = { index_weights_buffer };

                dt_roi_t roi_in_1 = (dt_roi_t){ .wd = round16(FEATURES_COUNT[5-i]) * size * size / 4, .ht = 1 };
                dt_roi_t roi_in_2 = (dt_roi_t){ .wd = round16(FEATURES_COUNT[4-i]) * size * size, .ht = 1 };

                decoder_convolution_layers[i][j] = dt_node_add(
                    graph, module, "tfcnn", shader,
                    size / 8 * DT_LOCAL_SIZE_X, size / 8 * DT_LOCAL_SIZE_Y, 1,
                    sizeof(pc), pc, 4,
                    "weights", "read", "ssbo", "f16", &module->connector[0].roi,
                    "output", "write", "ssbo", "f16", &roi_out,
                    "input1", "read", "ssbo", "f16", &roi_in_1,
                    "input2", "read", "ssbo", "f16", &roi_in_2
                );
            } else {
                char* shader = NULL;
                if (i == 0) shader = "conv7";
                if (i == 1) shader = "conv9";
                if (i == 2) shader = "conv11";
                if (i == 3) shader = "conv13";
                if (i == 4) shader = "conv15";

                int pc[] = { index_weights_buffer };

                dt_roi_t roi_in = (dt_roi_t){ .wd = round16(input_features_count) * size * size, .ht = 1 };

                decoder_convolution_layers[i][j] = dt_node_add(
                    graph, module, "tfcnn", shader,
                    size / 8 * DT_LOCAL_SIZE_X, size / 8 * DT_LOCAL_SIZE_Y, 1,
                    sizeof(pc), pc, 3,
                    "weights", "read", "ssbo", "f16", &module->connector[0].roi,
                    "output", "write", "ssbo", "f16", &roi_out,
                    "input", "read", "ssbo", "f16", &roi_in
                );
            }

            index_weights_buffer += 9 * input_features_count * output_features_count + output_features_count;
            dt_connector_copy(graph, module, 0, decoder_convolution_layers[i][j], 0);
        }
    }

    dt_connector_copy(graph, module, 1, encoder_convolution_layers[0], 2);

    for (int i = 0; i < 5; i++) {
        CONN(dt_node_connect_named(graph, encoder_convolution_layers[i], "output", decoder_convolution_layers[4-i][0], "input2"));
        CONN(dt_node_connect_named(graph, encoder_convolution_layers[i], "output", encoder_convolution_layers[i+1], "input"));
    }

    CONN(dt_node_connect_named(graph, encoder_convolution_layers[5], "output", decoder_convolution_layers[0][0], "input1"));

    for (int i = 0; i < 5; i++) {
        CONN(dt_node_connect_named(graph, decoder_convolution_layers[i][0], "output", decoder_convolution_layers[i][1], "input"));

        if (i < 4) {
            CONN(dt_node_connect_named(graph, decoder_convolution_layers[i][1], "output", decoder_convolution_layers[i+1][0], "input1"));
        }
    }

    dt_connector_copy(graph, module, 2, decoder_convolution_layers[4][1], 1);
}
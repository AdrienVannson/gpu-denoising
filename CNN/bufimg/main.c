#include "modules/api.h"
#include "connector.h"

void modify_roi_out(dt_graph_t *graph, dt_module_t *module)
{
    module->connector[1].roi.full_wd = 1024;
    module->connector[1].roi.full_ht = 1024;
}

void modify_roi_in(dt_graph_t *graph, dt_module_t *module)
{
    module->connector[0].roi.ht = module->connector[0].roi.full_ht;
    module->connector[0].roi.wd = module->connector[0].roi.full_wd;

    module->connector[2].roi.ht = module->connector[2].roi.full_ht;
    module->connector[2].roi.wd = module->connector[2].roi.full_wd;
}

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    const int id = dt_node_add(
        graph, module, "bufimg", "main",
        1024 * DT_LOCAL_SIZE_X / 16, 1024 * DT_LOCAL_SIZE_Y / 16, 1,
        0, NULL, 3,
        "img_in", "read", "rgba", "f16", &module->connector[0].roi,
        "img_out", "write", "rgba", "f16", &module->connector[1].roi,
        "buff", "read", "ssbo", "f16", &module->connector[2].roi
    );

    for (int i = 0; i < 3; i++) {
        dt_connector_copy(graph, module, i, id, i);
    }
}

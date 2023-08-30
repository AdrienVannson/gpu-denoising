#include "modules/api.h"
#include "connector.h"

// The ROI negociation has two phases:
// - set the output full size
// - set the input real size, that may be lower

void modify_roi_out(dt_graph_t *graph, dt_module_t *module)
{
    if (module->connector[0].roi.full_wd != module->connector[1].roi.full_ht) {
        printf("!!! Invalid matrix size in matmul\n");
    }

    const int n = module->connector[0].roi.full_ht;
    const int m = module->connector[1].roi.full_wd;

    module->connector[2].roi.full_ht = n;
    module->connector[2].roi.full_wd = m;
}

void modify_roi_in(dt_graph_t *graph, dt_module_t *module)
{
    for (int i = 0; i < 2; i++) {
        module->connector[i].roi.ht = module->connector[i].roi.full_ht;
        module->connector[i].roi.wd = module->connector[i].roi.full_wd;
    }
}

// For the tiled method
/*const int WIDTH_WORKGROUP = 32;
const int HEIGHT_WORKGROUP = 32;*/

// For the staging method
const int WIDTH_WORKGROUP = 256;
const int HEIGHT_WORKGROUP = 256;

// For other methods
/*const int WIDTH_WORKGROUP = 16;
const int HEIGHT_WORKGROUP = 16;*/

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    // A is n * p, B is p * m
    const int n = module->connector[0].roi.full_ht;
    const int p = module->connector[0].roi.full_wd;
    const int m = module->connector[1].roi.full_wd;

    int params[] = {n, p, m};

    const int id_matmul = dt_node_add(
        graph, module, "matmul", "staging",
        n / HEIGHT_WORKGROUP * DT_LOCAL_SIZE_X, m / WIDTH_WORKGROUP * DT_LOCAL_SIZE_Y, 1,
        sizeof(params), params, 3,
        "a", "read", "ssbo", "f16", &module->connector[0].roi,
        "b", "read", "ssbo", "f16", &module->connector[1].roi,
        "c", "write", "ssbo", "f16", &module->connector[2].roi
    );

    dt_connector_copy(graph, module, 0, id_matmul, 0);
    dt_connector_copy(graph, module, 1, id_matmul, 1);
    dt_connector_copy(graph, module, 2, id_matmul, 2);
}

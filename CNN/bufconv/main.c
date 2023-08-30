#include "modules/api.h"
#include "connector.h"

void modify_roi_out(dt_graph_t *graph, dt_module_t *module)
{
    const int n = module->connector[0].roi.full_wd;

    module->connector[1].roi.full_wd = n;
    module->connector[1].roi.full_ht = 1;
}

void modify_roi_in(dt_graph_t *graph, dt_module_t *module)
{
    const int n = module->connector[1].roi.wd;

    module->connector[0].roi.wd = n;
    module->connector[0].roi.ht = 1;
}

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    const int n = module->connector[0].roi.full_wd;

    int params[] = {n};

    const int id = dt_node_add(
        graph, module, "bufconv", "conv",
        n / 32 * DT_LOCAL_SIZE_X, 1, 1,
        sizeof(params), params, 2,
        "buf_in", "read", "ssbo", "f32", &module->connector[0].roi,
        "buf_out", "write", "ssbo", "f16", &module->connector[1].roi
    );

    dt_connector_copy(graph, module, 0, id, 0);
    dt_connector_copy(graph, module, 1, id, 1);
}
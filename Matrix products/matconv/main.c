#include "modules/api.h"
#include "connector.h"

void modify_roi_out(dt_graph_t *graph, dt_module_t *module)
{
    const int n = module->connector[0].roi.full_ht;
    const int m = module->connector[0].roi.full_wd;

    module->connector[1].roi.full_ht = n;
    module->connector[1].roi.ht = n;

    module->connector[1].roi.full_wd = m;
    module->connector[1].roi.wd = m;
}

void modify_roi_in(dt_graph_t *graph, dt_module_t *module)
{
    const int n = module->connector[1].roi.full_ht;
    const int m = module->connector[1].roi.full_wd;

    module->connector[0].roi.full_ht = n;
    module->connector[0].roi.ht = n;

    module->connector[0].roi.full_wd = m;
    module->connector[0].roi.wd = m;
}

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    const int n = module->connector[0].roi.full_ht;
    const int m = module->connector[0].roi.full_wd;

    int params[] = {n, m};

    const int id = dt_node_add(
        graph, module, "matconv", "conv",
        n / 16 * DT_LOCAL_SIZE_X, m / 16 * DT_LOCAL_SIZE_Y, 1,
        sizeof(params), params, 2,
        "mat_in", "read", "ssbo", "f32", &module->connector[0].roi,
        "mat_out", "write", "ssbo", "f16", &module->connector[1].roi
    );

    dt_connector_copy(graph, module, 0, id, 0);
    dt_connector_copy(graph, module, 1, id, 1);
}
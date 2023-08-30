#include "modules/api.h"
#include "connector.h"

void modify_roi_in(dt_graph_t *graph, dt_module_t *module)
{
    for (int i = 0; i < 4; i++) {
        module->connector[i].roi.ht = module->connector[i].roi.full_ht;
        module->connector[i].roi.wd = module->connector[i].roi.full_wd;
    }
}

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    if (module->connector[2].roi.full_ht != module->connector[3].roi.full_ht
        || module->connector[2].roi.full_wd != module->connector[3].roi.full_wd) {
        printf("!!! Invalid size in matrix comparaison\n");
    }

    const int n = module->connector[2].roi.ht;
    const int m = module->connector[2].roi.wd;

    int params[] = {n, m};

    const int id = dt_node_add(
        graph, module, "matrieq", "main",
        1, 1, 1,
        sizeof(params), params, 4,
        "input", "read", "rgba", "f16", &module->connector[0].roi,
        "output", "write", "rgba", "f16", &module->connector[1].roi,
        "matA", "read", "ssbo", "f16", &module->connector[2].roi,
        "matB", "read", "ssbo", "f16", &module->connector[3].roi
    );

    for (int i = 0; i < 4; i++) {
        dt_connector_copy(graph, module, i, id, i);
    }
}

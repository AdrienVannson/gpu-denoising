#include "modules/api.h"
#include "modules/localsize.h"
#include "connector.h"

void create_nodes(dt_graph_t *graph, dt_module_t *module)
{
    int workgroup_x_count = 48;
    int workgroup_y_count = 48;

    const int id_matmul = dt_node_add(
        graph, module, "comaperf", "main",
        workgroup_x_count * DT_LOCAL_SIZE_X, workgroup_y_count * DT_LOCAL_SIZE_Y, 1,
        0, 0, 2,
        "input", "read", "rgba", "f16", &module->connector[0].roi,
        "output", "write", "rgba", "f16", &module->connector[1].roi
    );

    dt_connector_copy(graph, module, 0, id_matmul, 0);
    dt_connector_copy(graph, module, 1, id_matmul, 1);
}
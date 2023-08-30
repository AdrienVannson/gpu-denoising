#include "modules/api.h"
#include "connector.h"

int max(const int x, const int y)
{
    return x > y ? x : y;
}

void modify_roi_out(dt_graph_t* graph, dt_module_t* module)
{
    const int n = max(*dt_module_param_int(module, 0), 1);
    const int m = max(*dt_module_param_int(module, 1), 1);
    
    module->connector[0].roi.full_ht = n;
    module->connector[0].roi.ht = n;

    module->connector[0].roi.full_wd = m;
    module->connector[0].roi.wd = m;
}

int read_source(dt_module_t* mod, void* mapped, dt_read_source_params_t* p)
{
    const int n = *dt_module_param_int(mod, 0);
    const int m = *dt_module_param_int(mod, 1);

    // Generate the matrix
    float* matrix = (float*)mapped;
    for (int i = 0; i < n*m; i++) {
        matrix[i] = (float)rand() / (float)RAND_MAX;
    }

    return 0;
}

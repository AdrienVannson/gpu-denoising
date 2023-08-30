#include "modules/api.h"
#include "connector.h"

void modify_roi_out(dt_graph_t* graph, dt_module_t* module)
{
    const int N = 1 << *dt_module_param_int(module, 0);
    
    module->connector[0].roi.full_ht = N;
    module->connector[0].roi.full_wd = 1;
}

int read_source(dt_module_t* module, void* mapped, dt_read_source_params_t* p)
{    
    const int N = 1 << *dt_module_param_int(module, 0);
    
    srand(42);

    int* perm = (int*)mapped;
    for (int i = 0; i < N; i++) {
        perm[i] = i;
    }

    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        const int temp = perm[i];
        perm[i] = perm[j];
        perm[j] = temp;
    }

    return 0;
}

#include "modules/api.h"
#include "connector.h"

void modify_roi_out(dt_graph_t* graph, dt_module_t* module)
{
    const char *filename = dt_module_param_string(module, 0);
    printf("%s\n", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("!!! Failed to open the file.\n");
        return;
    }

    // Read the size of the buffer
    int n;
    fscanf(file, "%d", &n);
    fclose(file);

    module->connector[0].roi.full_wd = n;
    module->connector[0].roi.wd = n;

    module->connector[0].roi.full_ht = 1;
    module->connector[0].roi.ht = 1;
}

int read_source(dt_module_t* mod, void* mapped, dt_read_source_params_t* p)
{
    const char *filename = dt_module_param_string(mod, 0);
    printf("%s\n", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("!!! Failed to open the file.\n");
        return 0;
    }

    // Read the size of the buffer
    int n;
    fscanf(file, "%d", &n);

    // Read the buffer from the file
    float* matrix = (float*)mapped;
    for (int i = 0; i < n; i++) {
        fscanf(file, "%f", &matrix[i]);
    }

    fclose(file);

    return 0;
}

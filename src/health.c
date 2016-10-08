#include "health.h"

int health_steps_today = 0;


void update_health_data(){
    HealthMetric metric = HealthMetricStepCount;
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    // Check the metric has data available for today
    HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
      start, end);

    if(mask & HealthServiceAccessibilityMaskAvailable) {
      // Data is available!
      health_steps_today = (int)health_service_sum_today(metric);
      APP_LOG(APP_LOG_LEVEL_INFO, "Steps today: %d", 
              health_steps_today);
    } else {
      // No data recorded yet today
      APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
    }
}



int health_get_steps_today(){
    return health_steps_today;
}


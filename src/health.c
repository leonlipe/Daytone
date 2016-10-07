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

static void health_handler(HealthEventType event, void *context) {
  // Which type of event occurred?
  switch(event) {
    case HealthEventSignificantUpdate:
      update_health_data();
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventSignificantUpdate event");
      break;
    case HealthEventMovementUpdate:
      update_health_data();
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventMovementUpdate event");
      break;
    case HealthEventSleepUpdate:
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventSleepUpdate event");
      break;
      case HealthEventMetricAlert:
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventMetricAlert event");
      break;
    case HealthEventHeartRateUpdate:
      APP_LOG(APP_LOG_LEVEL_INFO,
              "New HealthService HealthEventHeartRateUpdate event");
      break;
  }
}

void health_init() {
    #if defined(PBL_HEALTH)
    // Attempt to subscribe 
    if(!health_service_events_subscribe(health_handler, NULL)) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    }
    #else
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    #endif
      
}

int health_get_steps_today(){
    return health_steps_today;
}


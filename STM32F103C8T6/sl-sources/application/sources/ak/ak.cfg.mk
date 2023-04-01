# sizeof common message pool.
COMMON_MSG_POOL_SIZE = -DAK_COMMON_MSG_POOL_SIZE=6

# sizeof data common message.
COMMON_MSG_DATA_SIZE = -DAK_COMMON_MSG_DATA_SIZE=64

# sizeof pure message pool.
PURE_MSG_POOL_SIZE = -DAK_PURE_MSG_POOL_SIZE=6

# sizeof dynamic message pool.
DYNAMIC_MSG_POOL_SIZE = -DAK_DYNAMIC_MSG_POOL_SIZE=4

# sizeof timer pool: counting from timer_set() function called
TIMER_POOL_SIZE = -DAK_TIMER_POOL_SIZE=6

# Task objects log queue enable
TASK_OBJ_LOG_ENABLE = -DAK_TASK_OBJ_LOG_ENABLE

GENERAL_FLAGS += \
	$(COMMON_MSG_POOL_SIZE) \
	$(COMMON_MSG_DATA_SIZE) \
	$(PURE_MSG_POOL_SIZE) \
	$(DYNAMIC_MSG_POOL_SIZE) \
	$(DYNAMIC_DATA_POOL_SIZE) \
	$(DYNAMIC_PDU_SIZE) \
	$(TIMER_POOL_SIZE) \
	$(TASK_OBJ_LOG_ENABLE) \
	$(LOG_AK_KERNEL_ENABLE) \
	$(IRQ_OBJ_LOG_ENABLE) \

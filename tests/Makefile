WORK_DIR := samples
SUB_DIR := $(wildcard $(WORK_DIR)/*)

clean:
	@for dir in ${SUB_DIR}; do \
		echo $$dir; \
		${MAKE} -C $$dir clean; \
	done

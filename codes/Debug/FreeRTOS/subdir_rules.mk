################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/%.o: ../FreeRTOS/%.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mouli/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-6-2017-q1-update/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -DTARGET_IS_TM4C123_RB1 -I"/home/mouli/ti/ccsv7/tools/compiler/gcc-arm-none-eabi-6-2017-q1-update/arm-none-eabi/include" -I"/home/mouli/workspace_v7/freertos_on_tm4c123" -I"/home/mouli/ti/TivaWare_C_Series-2.1.4.178" -I"/home/mouli/workspace_v7/freertos_on_tm4c123/FreeRTOS/include" -I"/home/mouli/workspace_v7/freertos_on_tm4c123/FreeRTOS/portable/GCC/ARM_CM4F" -Os -ffunction-sections -fdata-sections -fsingle-precision-constant -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '



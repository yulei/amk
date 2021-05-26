
@rem launch the cmsis configuration tools

@if [%1]==[] (
    @set wizard=.\nRF5_SDK_17.0.2_d674dde\external_tools\cmsisconfig\CMSIS_Configuration_Wizard.jar
) else (
    @set wizard=%1
)

@if [%2]==[] (
    @set config=sdk_config.h
) else (
    @set config=%2
)

java -jar %wizard% %config%

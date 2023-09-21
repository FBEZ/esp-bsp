#IDF version is less than IDF5.0
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_LESS "5.0")
    set(SRC_VER)
else()
    set(SRC_VER)
endif()

idf_component_register(
    SRCS "esp_eye_idf5.c" "esp_eye_idf4.c" "esp_eye.c" ${SRC_VER}
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES driver spiffs
    PRIV_REQUIRES fatfs esp_lcd
)

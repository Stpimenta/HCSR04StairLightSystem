# Inclua todos os arquivos .cpp no diretório principal e em hardware/led_controller
file(GLOB_RECURSE SRC_FILES "*.cpp")

idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS "."
)


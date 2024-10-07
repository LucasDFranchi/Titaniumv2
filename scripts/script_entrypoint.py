Import("env")

# Run all compile scripts
env.Execute("$PYTHONEXE ./scripts/board_compiler.py \
            -i board_config.json \
            -b TITANIUM \
            -o ./lib/TitaniumOS/HAL/inc/ \
            ")

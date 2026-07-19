Import("env")

# override compilation DB path
env.Replace(COMPILATIONDB_PATH="compile_commands.json")
Import("env") # type: ignore[name-defined]

# override compilation DB path
env.Replace(COMPILATIONDB_PATH="compile_commands.json") # type: ignore[name-defined]
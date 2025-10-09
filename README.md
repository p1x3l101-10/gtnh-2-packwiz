# gtnh-2-packwiz
A project that just barely works

It downloads the files from DreamAssemblerXXL and the main modpack repo, and assembles them into a format that packwiz-complient clients can read

# Dependancies
See [The dependancies file](./DEPENDANCIES.md) for the list
Or just build with nix, this repo provides a flake

# Building
You need the dependancies, and if you feel inclined, you can use the cmake workflow.
Otherwise it is just a normal cmake build

# Build options
- `CMAKE_BUILD_TYPE`
    - `Release` or `Debug`, sets some internal values that are helpful for development
- `THREAD_POOL_MAX_THREADS`
    - The maximum threads that the program can spawn. At most it should only be using 2, but the default is 4 anyways
- `PACKWIZ_HASH_FORMAT`
    - The format to hash files to. As of writing, only `sha256` is supported, but in theory anything that `libcrypto++` supports should be doable
- `GTNH_AUTHOR`
    - I didn't want to use a constant for it, unless you have good reason it should stay as the preset
- `USING_UNSUP`
    - ON or OFF, enables some codepaths that do extra heavy lifting for generating [unsup](https://git.sleeping.town/unascribed/unsup) files. see the [config file](./gtnh-2-packwiz/defaultConfigs.toml) for more info
- `UNSUP_MINIMUM_VERSION`
    - the version of unsup to list in the `pack.toml` file. Should just be left as default unless you want a newer version than is the default
    - Has no effect without `USING_UNSUP=ON`
- `EXCLUSIVLY_TARGET_CLIENT`
    - A bandaid-fix for the whole "client only config files" thing that exists in gtnh
    - Ideally, I would use a meta file or just do some logic in unsup to exclude files from the server environment
- `MULTITHREADED_LOGS`
    - Helps debug any race conditions or bugs that arise in subthreads
    - Just enables an extra block in the log messages that tell you the thread id where it came from
    - Disabled by default

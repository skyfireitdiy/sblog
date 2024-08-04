add_rules("mode.debug", "mode.release")

add_requires("sqlite3", "zlib", "sqlite_orm", "digestpp", "cryptopp")

target("blog_server")
-- add_cxxflags("-Wall", "-Werror")
set_toolchains("clang")
set_kind("binary")
add_files("src/*.cpp")
add_files("third_part/sflib/src/sflib/**/*.cpp")
set_languages("c++20")
add_includedirs("include", "third_part/sflib/include")
add_packages("sqlite3", "zlib", "sqlite_orm", "digestpp", "cryptopp")

after_link(function(target)
    os.cp(target:targetfile(), path.join(os.projectdir(), "blog_run_docker"))
    os.cp(path.join(os.projectdir(), "config.json.example"),
    "blog_run_docker/config.json")
    os.cp(path.join(os.projectdir(), "static"), "blog_run_docker/static")
    os.cp(path.join(os.projectdir(), "template"), "blog_run_docker/template")
    os.cp(path.join(os.projectdir(), "db"), "blog_run_docker/db")
end)

on_package(function(target)
    os.exec("docker build -t blog_build_docker ./blog_build_docker")
    os.exec(format(
    [[docker run --rm -v%s:/sblog/ blog_build_docker bash -c "cd /sblog/ && rm -rvf ./blog_run_docker/blog_server ./blog_run_docker/static ./blog_run_docker/template ./blog_run_docker/config.json .xmake build && xmake f -m release --root -v && xmake --root -v"]],
    os.projectdir()))
    os.exec([[docker build -t wmb521/blog_run_docker ./blog_run_docker]])
    os.exec(format(
    [[docker run --rm -v%s:/sblog/ blog_build_docker bash -c "cd /sblog && rm -rvf ./blog_run_docker/blog_server ./blog_run_docker/static ./blog_run_docker/template ./blog_run_docker/config.json build .xmake"]],
    os.projectdir()))
end)

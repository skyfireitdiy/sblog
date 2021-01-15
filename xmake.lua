add_rules("mode.debug", "mode.release")

target("blog_server")

    add_cxxflags("-Wall", "-Werror")

    set_toolchains("clang")

    set_kind("binary")

    add_files("src/main.cpp")
    add_files("src/blog_server.cpp")
    add_files("src/database.cpp")
    add_files("src/config_manager.cpp")

    set_languages("c++20")

    add_includedirs("include", "third_part/inja/include/inja",
                    "third_part/inja/third_party/include", "third_part/sflib",
                    "third_part/digestpp",
                    "third_part/sqlite_orm/include/sqlite_orm")

    add_links("pthread", "ssl", "crypto", "z", "stdc++fs", "sqlite3")

    after_link(function (target)
        os.cp(target:targetfile(), path.join(os.projectdir(), "blog_run_docker"))
        os.cp(path.join(os.projectdir(), "config.json.example"), "blog_run_docker/config.json")
        os.cp(path.join(os.projectdir(), "static"), "blog_run_docker/static")
        os.cp(path.join(os.projectdir(), "template"), "blog_run_docker/template")
    end)

    on_package(function(target)
        os.exec("docker build -t blog_build_docker ./blog_build_docker")
        os.exec(format([[docker run --rm -v%s:/my_blog/ blog_build_docker bash -c "cd /my_blog/ && rm -rvf ./blog_run_docker/blog_server ./blog_run_docker/static ./blog_run_docker/template ./blog_run_docker/config.json .xmake build && xmake f -m release --root -v && xmake --root -v"]], os.projectdir()))
        os.exec([[docker build -t wmb521/blog_run_docker ./blog_run_docker]])
        os.exec(format([[docker run --rm -v%s:/my_blog/ blog_build_docker bash -c "cd /my_blog && rm -rvf ./blog_run_docker/blog_server ./blog_run_docker/static ./blog_run_docker/template ./blog_run_docker/config.json build .xmake"]], os.projectdir()))
    end)

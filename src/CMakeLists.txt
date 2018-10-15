add_executable(vhdl-ls
    VHDLLSMain.cpp
)

target_link_libraries(vhdl-ls vc lsp asio pthread)
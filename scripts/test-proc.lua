-- Example of running a procedual code from a lua script.

code = [=[
shape branch
rule {
    cylinder [sz 0.7]
    branch [rz 0 z 0.25 s 0.9 rx 30 z 0.25
            light -0.1 hue 10 sat 0.1]
}
rule 0.2 {
branch [rz 180]
}
rule 0.2 {
    branch [rz 90]
}
rule 0.1 {
    branch [rx -90 s 0.8 z 1]
    branch []
}

shape main {
    [light 1 antialiased 1]
    branch [s 32]
}
]=]

mesh = Mesh:new()
proc = Proc:new(code)
proc:run(mesh)
mesh:save('./out.gox')

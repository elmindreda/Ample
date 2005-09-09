
noinst_PROGRAMS = hello lint render

hello_CPPFLAGS = -I@top_srcdir@/include
hello_LDFLAGS = -L@top_srcdir@/src
hello_LDADD = -lample -lverse
hello_SOURCES = hello.cpp

lint_CPPFLAGS = -I@top_srcdir@/include
lint_LDFLAGS = -L@top_srcdir@/src
lint_LDADD = -lample -lverse
lint_SOURCES = lint.cpp

render_CPPFLAGS = -I@top_srcdir@/include
render_LDFLAGS = -L@top_srcdir@/src -framework OpenGL -framework AGL -framework Carbon
render_LDADD = -lample -lverse -lglfw 
render_SOURCES = render.cpp


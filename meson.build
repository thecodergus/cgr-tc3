# Projeto de exemplo para uso do GenICam com C
project('Aplicação', 'c')

# Inicia as flags de compilação como um vetor vazio
c_args = []

# Adiciona flags de compilação para o GCC e Clang
cc = meson.get_compiler('c')
if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
 c_args += '-Wno-deprecated'
endif

# Importa as pastas de cabeçalho
inc = include_directories('include', '/usr/local/include')

# Importa os arquivos de código fonte
app_src = files(
 'src/main.c',
 'src/robo.c'
)

compiler = meson.get_compiler('c')

# Local para importar as dependencias
openGL_dep = dependency('gl')
glew_dep = dependency('glew', required: false)
gluL_dep = dependency('glu')
glut_dep = dependency('glut', required: false)
thread_dep = dependency('threads')
math_dep = cc.find_library('m', required : false)



libsPath = '/usr/lib/x86_64-linux-gnu/'
if not glut_dep.found()
  glut_dep = compiler.find_library('glut', dirs: libsPath)
endif
if math_dep.found()
  add_project_link_arguments('-lm', language : 'c')
endif

# Aplicação Central
executable('main',
   sources : [
     app_src
   ],
   include_directories : inc,
   dependencies: [
    openGL_dep, 
    glew_dep,
    glut_dep, 
    gluL_dep, 
    thread_dep,
    math_dep
   ],
   install: true,
   c_args : c_args
 )
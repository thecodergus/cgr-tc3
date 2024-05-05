#! /bin/bash

# Check if meson is installed
if ! pip3 show meson &> /dev/null; then
    echo "meson is not installed. Installing now..."
    pip3 install meson
    # Reload bash source
    echo "Reloading bash source..."
    source ~/.bashrc
fi


NOME_PROJETO="main" # Nome do projeto

# Configurações
export CC=gcc # Escolhendo o compilador
export CMAKE_PREFIX_PATH="/opt/vcpkg"

export ninja=./ninja

PASTA="build"

# Verificando se a pasta existe
if [ -d "$PASTA" ]; then
    meson setup $PASTA --reconfigure # Reconfigurando o meson se a pasta já existe
else
    meson setup $PASTA # Configurando o meson se a pasta não existe
fi

cd $PASTA # Entrando na pasta build

# Compilando o codigo
if ninja; then
    # clear -x # Limpando a tela
    ./$NOME_PROJETO # Executando o codigo compilado
else
    echo "Erro na compilação. O projeto não será executado."
fi
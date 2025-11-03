# ===============================
# Etapa base: Ubuntu con compilador
# ===============================
FROM ubuntu:24.04

# Evita preguntas interactivas durante la instalación
ENV DEBIAN_FRONTEND=noninteractive

# ===============================
# Instalar dependencias principales
# ===============================
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    python3 \
    python3-pip \
    unzip \
    libncurses5-dev \
    libncursesw5-dev \
    && rm -rf /var/lib/apt/lists/*

# ===============================
# Instalar PDCurses
# ===============================
WORKDIR /tmp
RUN git clone https://github.com/wmcbrine/PDCurses.git && \
    cd PDCurses && \
    make -C pdcurses && \
    make -C pdcurses install PREFIX=/usr/local && \
    cd .. && rm -rf PDCurses

# ===============================
# Instalar librerías Python
# ===============================
RUN pip install --no-cache-dir pandas scikit-learn matplotlib

# ===============================
# Configurar entorno del proyecto
# ===============================
WORKDIR /app
COPY . .

# ===============================
# Construir el proyecto C++ con CMake
# ===============================
RUN mkdir build && cd build && \
    cmake .. && make

# ===============================
# Comando por defecto al iniciar el contenedor
# ===============================
WORKDIR /app/build
CMD ["./PP3_Proyecto"]

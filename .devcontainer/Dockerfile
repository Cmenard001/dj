FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

# Base build tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    ca-certificates \
    sudo \
    # ARM bare-metal toolchain (STM32F4)
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    # OpenOCD for STM32 flashing/debugging
    openocd \
    # GDB with multi-arch support (Linux & remote targets)
    gdb-multiarch \
    # Doxygen + Graphviz for documentation generation (CMake `doxygen` target / CI Pages)
    doxygen \
    graphviz \
    # SSH client for remote deployment to STM32MP15
    openssh-client \
    # Python for scripts (encoding_converter.py, dj_playground_vizualizer.py)
    python3 \
    python3-pip \
    python3-matplotlib \
    && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# STM32MP15 / Roboteseo SDK
# The STM32MP15 target requires the Roboteseo Poky SDK installed at
#   /opt/roboteseo/1.0/
# Mount or copy the SDK into the container, then source the environment
# script before running cmake with the stm32mp15_preset:
#   source /opt/roboteseo/1.0/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
# ---------------------------------------------------------------------------

# Create a non-root user matching typical host UID/GID
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000
RUN if getent group ${USER_GID} > /dev/null 2>&1; then \
        groupmod -n ${USERNAME} $(getent group ${USER_GID} | cut -d: -f1); \
    else \
        groupadd --gid ${USER_GID} ${USERNAME}; \
    fi \
    && if getent passwd ${USER_UID} > /dev/null 2>&1; then \
        usermod -l ${USERNAME} -d /home/${USERNAME} -m $(getent passwd ${USER_UID} | cut -d: -f1); \
    else \
        useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME}; \
    fi \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

USER ${USERNAME}
WORKDIR /workspace

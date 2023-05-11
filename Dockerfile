FROM espressif/idf:v5.0.2

ENV DEBIAN_FRONTEND="noninteractive"

RUN \
  --mount=type=cache,target=/var/cache/apt \
  --mount=type=cache,target=/var/lib/apt/lists \
  apt-get -qq update && \
  apt-get -qq install \
	git \
	libusb-1.0-0 \
	python3 \
	python3-pip \
	python-is-python3 \
	python3-venv \
	tio \
	sudo \
	nano

RUN useradd --create-home --uid 1000 build
COPY --chown=1000 container.gitconfig /home/build/.gitconfig

ENV PATH="$PATH:/sallow/.local/bin"
WORKDIR /sallow

ENV ADF_VER="c1e884871872bc748a0248746fcba4980170356f"
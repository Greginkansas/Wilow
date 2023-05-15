FROM espressif/idf:release-v4.4

ENV DEBIAN_FRONTEND="noninteractive"

RUN \
  --mount=type=cache,target=/var/cache/apt \
  --mount=type=cache,target=/var/lib/apt/lists \
  apt-get -qq update && \
  apt-get -qq install \
	git \
	libusb-1.0-0 \
	nano \
	python-is-python3 \
	python3 \
  python3-num2words \
	python3-pip \
	python3-requests \
	python3-venv \
	sudo \
	tio

RUN pip install --upgrade pyclang

RUN useradd --create-home --uid 1000 build
COPY --chown=1000 container.gitconfig /home/build/.gitconfig

ENV PATH="$PATH:/willow/.local/bin"
WORKDIR /willow

ENV ADF_VER="v2.5"
RUN \
  cd /opt/esp/idf && \
  curl https://raw.githubusercontent.com/espressif/esp-adf/$ADF_VER/idf_patches/idf_v4.4_freertos.patch | patch -p1

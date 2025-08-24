ARG CF_SRC_OS_IMAGE=need_a_src_os_image

FROM ${CF_SRC_OS_IMAGE}

#
ARG CF_OUTP_IMAGE=need_an_output_os_image
ENV DEFAULT_DOCKCROSS_IMAGE=${CF_OUTP_IMAGE}

#
ENV DEBIAN_FRONTEND=noninteractive

# libmpg123 from https://sourceforge.net/projects/mpg123/
ENV TMP_DN=mpg123-1.33.2
COPY files/${TMP_DN}.tar.xz files/compile_n_install-mpg123.sh /root/

RUN \
	cd /root \
	&& tar xf ${TMP_DN}.tar.xz \
	&& rm ${TMP_DN}.tar.xz \
	&& cd ${TMP_DN} \
	&& ../compile_n_install-mpg123.sh \
	&& cd .. \
	&& rm -r ${TMP_DN} compile_n_install-mpg123.sh

# libogg from https://github.com/xiph/ogg
ENV TMP_DN=libogg-1.3.6
COPY files/${TMP_DN}.tar.xz files/compile_n_install-ogg.sh /root/

RUN \
	cd /root \
	&& tar xf ${TMP_DN}.tar.xz \
	&& rm ${TMP_DN}.tar.xz \
	&& cd ${TMP_DN} \
	&& ../compile_n_install-ogg.sh \
	&& cd .. \
	&& rm -r ${TMP_DN} compile_n_install-ogg.sh

# libvorbis from https://github.com/xiph/vorbis
ENV TMP_DN=libvorbis-1.3.7
COPY files/${TMP_DN}.tar.xz files/compile_n_install-vorbis.sh /root/

RUN \
	cd /root \
	&& tar xf ${TMP_DN}.tar.xz \
	&& rm ${TMP_DN}.tar.xz \
	&& cd ${TMP_DN} \
	&& ../compile_n_install-vorbis.sh \
	&& cd .. \
	&& rm -r ${TMP_DN} compile_n_install-vorbis.sh

# zlib from https://github.com/madler/zlib
ENV TMP_DN=zlib-1.3.1
COPY files/${TMP_DN}.tar.xz files/compile_n_install-zlib.sh /root/

RUN \
	cd /root \
	&& tar xf "${TMP_DN}.tar.xz" \
	&& rm "${TMP_DN}.tar.xz" \
	&& cd "${TMP_DN}" \
	&& ../compile_n_install-zlib.sh \
	&& cd .. \
	&& rm -r "${TMP_DN}" compile_n_install-zlib.sh

# add alias ll
RUN \
	echo "alias ll='ls -l'" >> /etc/bash.bashrc

# install 'less'
RUN \
	apt install --no-install-recommends -y \
		less

#
RUN \
	rm -rf \
			/usr/share/man \
			/usr/share/doc \
			/usr/share/icons \
			/usr/share/poppler \
			/usr/share/mime

#
ENV DEBIAN_FRONTEND=dialog

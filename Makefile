# Makefile for the Minix servers
.include <bsd.own.mk>

SUBDIR=	ds inet is mfs pfs pm rs sched vfs vm

.if ${MKIMAGEONLY} == "yes"
# don't build the experimental servers
.else
SUBDIR+=	ext2 hgfs
.endif

# Add the calc server to the build
SUBDIR+=	calc

.include <bsd.subdir.mk>

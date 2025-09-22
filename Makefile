.include <bsd.own.mk>

SUBDIR+=	calc ds input mib pm rs sched vfs vm

.if ${MKIMAGEONLY} == "no"
SUBDIR+=	ipc is devman
.endif

.include <bsd.subdir.mk>

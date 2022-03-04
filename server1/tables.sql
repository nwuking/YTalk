/* 用户信息表 */
CREATE TABLE IF NOT EXISTS user (
    u_id BIGINT(20) NOT NULL COMMENT '用户id',
    u_name CHAR(11) NOT NULL COMMENT '用户名',
    u_password VARCHAR(64) DEFAULT NULL COMMENT '用户密码',
    u_nickname VARCHAR(64) NOT NULL COMMENT '用户昵称',
    u_gender CHAR(1) DEFAULT '0' COMMENT '用户性别',
    u_birthday BIGINT(20) DEFAULT 20220101 COMMENT '生日',
    u_signature VARCHAR(256) DEFAULT NULL COMMENT '个性签名',
    u_facetype INT(10) DEFAULT 0 COMMENT '用户头像类型',
    u_face VARCHAR(64) DEFAULT NULL COMMENT '用户头像',
    u_rg_time DATETIME NOT NULL COMMENT '注册时间',
    g_owner_id BIGINT(20) DEFAULT 0 COMMENT '群主id',
    u_teaminfo VARCHAR(2000) DEFAULT NULL COMMENT '好友分组信息',
    PRIMARY KEY (u_id),
    INDEX u_id(u_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*用户关系表 */
CREATE TABLE IF NOT EXISTS relationship(
    id BIGINT(20) NOT NULL AUTO_INCREMENT COMMENT '自增id',
    u_id1 BIGINT(20) NOT NULL COMMENT '用户A',
    u_id2 BIGINT(20) NOT NULL COMMENT '用户B',
    u_id1_team VARCHAR(32) NOT NULL DEFAULT "my friends" COMMENT '用户B在用户A的分组名',
    u_id1_remarks VARCHAR(20) COMMENT '用户B在用户A的备注',
    u_id2_team VARCHAR(20) NOT NULL DEFAULT "my friends" COMMENT '用户A在用户B的分组名',
    PRIMARY KEY (id),
    KEY id (id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

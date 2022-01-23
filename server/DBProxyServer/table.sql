/*================================================================================   
 *    Date: 2022-1-23
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/


/* 用户信息表 */
CREATE TABLE IF NOT EXISTS user (
    u_id INT NOT NULL COMMENT '用户id',
    u_name CHAR(10) NOT NULL COMMENT '用户名',
    u_nickname VARCHAR(50) NOT NULL COMMENT '用户昵称',
    u_password VARCHAR(50) NOT NULL COMMENT '用户密码',
    u_gender CHAR(1) DEFAULT '0' COMMENT '用户性别',
    u_birthday BIGINT(20) DEFAULT 20220101 COMMENT '生日',
    u_picture VARCHAR(50) DEFAULT NULL COMMENT '头像',
    u_signature VARCHAR(256) DEFAULT NULL COMMENT '个性签名',
    u_rg_time DATETIME NOT NULL COMMENT '注册时间',
    u_friends VARCHAR(2000) DEFAULT NULL COMMENT '好友列表',
    u_groups VARCHAR(2000) DEFAULT NULL COMMENT '群列表',
    PRIMARY KEY (u_id),
    INDEX u_id(u_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
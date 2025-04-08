#!/bin/bash -e
COMMUNITY="bravo"

#-------------------------------------------------------
#  Part 1: 参数处理增强
#-------------------------------------------------------
# 新增参数解析逻辑
DEPTH_THRESH=25     # 默认深度阈值
RETURN_DIST=200     # 默认返航距离
TIME_WARP=1         # 时间扭曲系数

for ARGI; do
    case "$ARGI" in
        --help|-h)
            echo "用法: $0 [时间扭曲] [深度阈值] [返航距离]"
            echo "示例: ./launch.sh 1 30 250"
            exit 0
            ;;
        # 数字参数顺序处理
        [0-9]*)
            if [ $TIME_WARP -eq 1 ]; then
                TIME_WARP=$ARGI
            elif [ $DEPTH_THRESH -eq 25 ]; then
                DEPTH_THRESH=$ARGI
            else
                RETURN_DIST=$ARGI
            fi
            ;;
        *)
            echo "错误参数: $ARGI"
            exit 1
            ;;
    esac
done

#-------------------------------------------------------
#  Part 2: 动态配置生成
#-------------------------------------------------------
# 生成临时MOOS配置（深度阈值）
cat > dynamic_odometry.moos <<EOF
ProcessConfig = pOdometry {
  depth_thresh = $DEPTH_THRESH
}
EOF

# 生成临时行为配置（返航距离）
sed "s/ODOMETRY_DIST_AT_DEPTH >= 200/ODOMETRY_DIST_AT_DEPTH >= $RETURN_DIST/" \
    bravo.bhv > bravo_dynamic.bhv

#-------------------------------------------------------
#  Part 3: 带动态配置启动
#-------------------------------------------------------
printf "启动参数：\n"
printf "时间扭曲: %s\n深度阈值: %s米\n返航距离: %s米\n" $TIME_WARP $DEPTH_THRESH $RETURN_DIST

pAntler bravo.moos \
    --MOOSFile=dynamic_odometry.moos \
    --bhv=bravo_dynamic.bhv \
    --MOOSTimeWarp=$TIME_WARP >& /dev/null &

#-------------------------------------------------------
#  Part 4: 清理与监控
#-------------------------------------------------------
uMAC -t bravo.moos
kill %1 2>/dev/null  # 安全终止进程
rm -f dynamic_odometry.moos bravo_dynamic.bhv  # 清理临时文件
echo "任务已终止，临时配置已清理"
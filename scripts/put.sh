
 make && (
./scripts/indent.sh
git commit -a
read -p "Push? "
./scripts/push.sh
) && exit 0

echo "failed to build, will not commit/push"
exit 1

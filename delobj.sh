#!/bin/bash

echo "Chọn một trong hai lựa chọn sau:"
echo "1. Xóa tất cả các tệp thực thi trong thư mục hiện tại."
echo "2. Xóa tất cả các tệp thực thi trong thư mục hiện tại mà không bao gồm các tệp trong các thư mục con."

read -p "Nhập lựa chọn của bạn (1 hoặc 2): " choice

case $choice in
    1)
        echo "Đang xóa tất cả các tệp thực thi trong thư mục hiện tại..."
        find . -type f -perm +111 -exec rm {} \;
        echo "Xóa hoàn tất."
        ;;
    2)
        echo "Đang xóa tất cả các tệp thực thi trong thư mục hiện tại mà không bao gồm các tệp trong các thư mục con..."
        find . -maxdepth 1 -type f -perm +111 -exec rm {} \;
        echo "Xóa hoàn tất."
        ;;
    *)
        echo "Lựa chọn không hợp lệ. Vui lòng chọn 1 hoặc 2."
        ;;
esac
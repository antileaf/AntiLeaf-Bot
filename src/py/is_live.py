# Code by KSkun, 2020/5

import requests
import json

def is_live(room_id):
    req = requests.get("https://api.live.bilibili.com/xlive/web-room/v1/index/getInfoByRoom?room_id=" + str(room_id)).content # 从页面上扒下来的 API 地址
    req_json = json.loads(req)
    live_status = req_json["data"]["room_info"]["live_status"] # 用 JSON 提取到想要的信息
    return live_status


if __name__ == "__main__":
    room_id = input()
    live_status = is_live(room_id)
    # live_status
    if live_status == 0:
        print("未开播")
    elif live_status == 1:
        print("直播中")
    else:
        print("轮播中")

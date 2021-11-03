import sys
import time

import adafruit_hue

from air import wifi
from secrets import HUE_IP, HUE_USERNAME


bridge = None


def _connect_bridge(force=False):
    global bridge
    if not bridge or force:
        print("Connecting to bridge...")
        bridge = adafruit_hue.Bridge(wifi, HUE_IP, HUE_USERNAME)


def _get_cycle_requests(lids, secs):
    """
    Returns a list of dictionaries that represent data for requests against hue
    API that represents lights within lids list of ids. This includes a
    transitiontime equal to the number secs passed in. Only lights that on are
    included in response. Errors are logged but otherwise ignored.
    """
    reqs = []
    for lid in lids:
        time.sleep(0.1)
        try:
            resp = get_light_state(lid)
        except Exception as exc:
            print(f"Light {lid} failed GET: {exc}")
        else:
            if resp["state"]["on"]:
                reqs.append({
                    "hue": resp["state"]["hue"],
                    "sat": resp["state"]["sat"],
                    "transitiontime": secs * 10 
                })
    return reqs


def cycle_lights(lids, secs, f_interrupt=lambda: False):
    """
    Lights update their state to the previous light in the chain
    every secs seconds (lights are individually added, so all will
    not be fully cycling until secs * len(lids)). An f_interrupt
    function may be passed in which is checked between requests.
    It is expected that this is a blocking function. If it resolves
    to True, cycle_lights exits immediately.

    For best results, f_interrupt should sleep for (secs / lids).

    # Simple cycle lights with button interrupt
    hue.cycle_lights(
        (1, 2, 3),
        6,
        lambda: sleep_interrupt(
            2,
            red_button
        )
    )
    """
    reqs = _get_cycle_requests(lids, secs)
    lid = 0
    sid = 1
    while True:
        while lid < len(reqs):
            if sid == len(reqs):
                sid = 0
            try:
                set_light_state(lids[lid], **reqs[sid])
            except Exception as exc:
                print(exc)
                time.sleep(0.2)
            lid += 1
            sid += 1
            if f_interrupt():
                return
        # Reset our light cursor and shift requests left.
        lid = 0
        reqs.append(reqs.pop(0))


def ensure_on(lid):
    """
    This is for validating that a light that should be on is still on. If there
    is a problem reading the response, it will default to returning True with a
    warning.
    """
    try:
        return get_light_state(lid)['state']['on'] is True
    except Exception as exc:
        print(exc)
        return True


def get_light_state(lid):
    """
    Simple print for on, bri, hue, sat.
    """
    print(f"GET {lid}")
    return bridge.get_light(lid)


def set_light_state(lid, **kwargs):
    """
    body must be bytes (dumped json).
    on - bool
    bri - brightness 1-254
    hue - hue 0-65535
    sat - saturation 0-254
    effect - none or "colorloop"
    transitiontime - default 4, 10=1sec
    bri_inc - -254-254
    sat_inc - -254-254
    hue_inc - -254-254
    """
    try:
        return bridge.set_light(lid, **kwargs)
    except Exception as exc:
        print(f"set_light_state {lid} {kwargs} failed: {exc}")
        sys.print_exception(exc)


def set_scene(group_id, scene_id):
    try:
        return bridge.set_scene(group_id, scene_id)
    except Exception as exc:
        print(f"set_scene {group_id} {scene_id} failed: {exc}")
        sys.print_exception(exc)


def set_group(group_id, **kwargs):
    try:
        return bridge.set_group(group_id, **kwargs)
    except Exception as exc:
        print(f"set_group {group_id} failed: {exc}")
        sys.print_exception(exc)


_connect_bridge()

package connect

const MESSAGE_PING byte = 1
const MESSAGE_ERROR byte = 2
const MESSAGE_EVENT byte = 3
const MESSAGE_CLOSE byte = 9

const EVENT_PAUSE byte = 1
const EVENT_MOUSE_CLICK byte = 2
const EVENT_UPDATE_RATE_INCREASE byte = 3
const EVENT_UPDATE_RATE_DECREASE byte = 4
const EVENT_GET byte = 5
const EVENT_SET_BORDERS byte = 6
const EVENT_UPDATE byte = 7
const EVENT_LOG byte = 8

const MAX_PACKET_LEN = 1000
const MAX_RETRIES = 1000
const MAX_TIMEOUT = 1 * 1000 * 1000 * 1000

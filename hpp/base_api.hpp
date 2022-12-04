#pragma once

enum class ORDER_SIDE{
	BUY,
	SELL,
	NONE
};

enum class TIME_IN_FORCE{
	GTC,
	IOC,
	FOK
};

enum class ORDER_TYPE{
	LIMIT,
	MARKET,
	STOP_LOSS,
	STOP_LOSS_LIMIT,
	TAKE_PROFIT,
	TAKE_PROFIT_LIMIT,
	LIMIT_MAKER
};

enum class SECURITY_TYPE{
	SIGNED,
	PUB,
	NONE
};

enum class INTERVAL{
	s1,
	m1,
	m3,
	m5,
	m15,
	m30,
	h1,
	h2,
	h4,
	h6,
	h8,
	h12,
	d1,
	d3,
	w1,
	M1
};
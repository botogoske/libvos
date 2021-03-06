//
// Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "DNSQuery.hh"

namespace vos {

const char* _DNS_HDR_TYPE_NAME[] = {
		"query"
	,	"response"
	};
const char* _DNS_HDR_OPCODE_NAME[] = {
		"QUERY"
	,	"IQUERY"
	,	"STATUS"
	};
const char* _DNS_HDR_RCODE_NAME[RCODE_SIZE + 1] = {
		"OK"
	,	"FORMAT_ERROR"
	,	"SERVER_FAILURE"
	,	"NAME_ERROR"
	,	"NOT_IMPLEMENTED"
	,	"REFUSED"
	,	"UNKNOWN"
	};

const char* DNSQuery::__cname = "DNSQuery";

/**
 * @method	: DNSQuery::DNSQuery
 */
DNSQuery::DNSQuery() : Buffer()
,	_id(0)
,	_flag(0)
,	_n_qry(0)
,	_n_ans(0)
,	_n_aut(0)
,	_n_add(0)
,	_q_len (0)
,	_q_type(0)
,	_q_class(0)
,	_name()
,	_bfr_type(BUFFER_IS_UDP)
,	_rr_ans()
,	_rr_aut()
,	_rr_add()
,	_rr_ans_p(NULL)
,	_rr_aut_p(NULL)
,	_rr_add_p(NULL)
,	_ans_ttl_max(0)
,	_attrs (DNS_IS_QUERY)
{}

/**
 * @method	: DNSQuery::~DNSQuery
 */
DNSQuery::~DNSQuery()
{}

/**
 * @method	: DNSQuery::set_buffer
 * @param	:
 *	> bfr	: pointer to DNS packet.
 *	> type	: type of DNS packet origin, TCP or UDP.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: set DNSQuery packet to 'bfr'.
 */
int DNSQuery::set(const Buffer* bfr, const int type)
{
	if (!bfr) {
		return -1;
	}
	if (type != BUFFER_IS_UDP && type != BUFFER_IS_TCP) {
		return -1;
	}
	if (_id) {
		reset();
	}

	_bfr_type = type;
	Error err = copy(bfr);
	if (err != NULL) {
		return -1;
	}

	return 0;
}

DNSQuery* DNSQuery::duplicate ()
{
	DNSQuery*	dup	= new DNSQuery ();

	if (! dup) {
		return NULL;
	}

	Error err = dup->copy_raw (_v, _i);
	if (err != NULL) {
		delete dup;
		dup = NULL;
		return NULL;
	}

	dup->_q_len		= _q_len;
	dup->_q_type		= _q_type;
	dup->_q_class		= _q_class;
	dup->_name.copy (&_name);
	dup->_ans_ttl_max	= _ans_ttl_max;
	dup->_attrs		= _attrs;

	return dup;
}

/**
 * @method	: DNSQuery::to_udp
 * @param	:
 *	> tcp	: pointer to DNS packet from TCP socket or buffer.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: set content of DNSQuery buffer to UDP packet using packet
 * from TCP. If 'tcp' is nil then convert the current buffer for UDP packet.
 */
int DNSQuery::to_udp(const Buffer *tcp)
{
	if (!tcp) {
		if (_bfr_type == BUFFER_IS_UDP) {
			return 0;
		}

		_i -= DNS_TCP_HDR_SIZE;
		memmove(_v, &_v[DNS_TCP_HDR_SIZE], _i);
		_v[_i] = 0;
	} else {
		Error err = copy_raw(tcp->v(DNS_TCP_HDR_SIZE)
				, tcp->len() - DNS_TCP_HDR_SIZE);
		if (err != NULL) {
			return -1;
		}
	}

	_bfr_type = BUFFER_IS_UDP;
	return 0;
}

/**
 * @method	: DNSQuery::to_tcp
 * @param	:
 *	> udp	: pointer to DNS packet from UDP socket or buffer.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: set content of DNSQuery buffer to TCP packet using packet
 * from UDP. If udp is nil then convert the current buffer for TCP packet.
 */
int DNSQuery::to_tcp(const Buffer* udp)
{
	uint16_t size;
	Error err;

	if (!udp) {
		if (_bfr_type == BUFFER_IS_TCP) {
			return 0;
		}

		err = shiftr(DNS_TCP_HDR_SIZE);
		if (err != NULL) {
			return -1;
		}

		size = (uint16_t)(_i - DNS_TCP_HDR_SIZE);
		size = htons(size);
		memcpy(_v, &size, DNS_TCP_HDR_SIZE);
	} else {
		err = resize(udp->len() + DNS_TCP_HDR_SIZE);
		if (err != NULL) {
			return -1;
		}
		reset();

		size = (uint16_t) udp->len();
		memcpy(_v, &size, DNS_TCP_HDR_SIZE);
		_i = DNS_TCP_HDR_SIZE;

		err = append(udp);
		if (err != NULL) {
			return -1;
		}
	}

	_bfr_type = BUFFER_IS_TCP;
	return 0;
}

void DNSQuery::set_header (uint16_t id, uint16_t flag, uint16_t n_qry
		, uint16_t n_ans
		, uint16_t n_aut
		, uint16_t n_add)
{
	_id		= htons (id);
	_flag		= htons (flag);
	_n_qry		= htons (n_qry);
	_n_ans		= htons (n_ans);
	_n_aut		= htons (n_aut);
	_n_add		= htons (n_add);

	memcpy(_v	, &_id		, 2);
	memcpy(&_v[2]	, &_flag	, 2);
	memcpy(&_v[4]	, &_n_qry	, 2);
	memcpy(&_v[6]	, &_n_ans	, 2);
	memcpy(&_v[8]	, &_n_aut	, 2);
	memcpy(&_v[10]	, &_n_add	, 2);
	_i = DNS_HDR_SIZE;

	_id		= ntohs(_id);
	_flag		= ntohs(_flag);
	_n_qry		= ntohs(_n_qry);
	_n_ans		= ntohs(_n_ans);
	_n_aut		= ntohs(_n_aut);
	_n_add		= ntohs(_n_add);
}

/**
 * @method	: DNSQuery::create_question
 * @param	:
 *	> qname	: a domain name.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: create question packet data to be send to server.
 */
int DNSQuery::create_question(const char* qname, const int type)
{
	if (!qname) {
		return -1;
	}

	size_t len = 0;
	Buffer	label;

	reset(DNSQ_DO_ALL);

	set_header ((uint16_t) (rand() % 65536)
		, HDR_IS_QUERY | OPCODE_QUERY
		, 1, 0, 0, 0);

	_q_type		= htons((uint16_t) type);
	_q_class	= htons(QUERY_C_IN);

	_name.copy_raw(qname);

	len = _name.len() + 16;

	if (len > _l) {
		Error err = resize(len);
		if (err != NULL) {
			return -1;
		}
	}

	_i = DNS_HDR_SIZE;
	DNS_rr::APPEND_DNS_LABEL (this, qname
				, (unsigned int) strlen (qname));

	append_bin(&_q_type, 2);
	append_bin(&_q_class, 2);

	_q_type		= ntohs(_q_type);
	_q_class	= ntohs(_q_class);

	return 0;
}

/**
 * @method	: DNSQuery::extract
 * @param extract_rr	: if !0 extract RR also.
 * @return 0	: success, or buffer is empty.
 * @return -1	: fail at extracting header.
 * @return -2	: fail at extracting question.
 * @return -3	: fail at extracting RR.
 * @desc	: extract contents of buffer (DNS packet).
 */
int DNSQuery::extract (const char extract_rr_flag)
{
	if (is_empty()) {
		return 0;
	}

	int	s	= 0;
	int	len	= 0;

	if (_bfr_type == BUFFER_IS_TCP) {
		s = to_udp();
		if (s < 0) {
			return -1;
		}
	}

	reset(DNSQ_DO_DATA_ONLY);

	s = extract_header();
	if (s < 0) {
		return -1;
	}

	len = extract_question();
	if (len <= 0) {
		return -2;
	}

	s = extract_resource_record (extract_rr_flag);
	if (s != 0) {
		return -3;
	}

	return 0;
}

/**
 * @method	: DNSQuery::extract_header
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: extract header of DNS packet.
 */
int DNSQuery::extract_header()
{
	if (_i < DNS_HDR_SIZE) {
		return -1;
	}

	memcpy(&_id	, _v		, 2);
	memcpy(&_flag	, &_v[2]	, 2);
	memcpy(&_n_qry	, &_v[4]	, 2);
	memcpy(&_n_ans	, &_v[6]	, 2);
	memcpy(&_n_aut	, &_v[8]	, 2);
	memcpy(&_n_add	, &_v[10]	, 2);

	_id	= ntohs(_id);
	_flag	= ntohs(_flag);
	_n_qry	= ntohs(_n_qry);
	_n_ans	= ntohs(_n_ans);
	_n_aut	= ntohs(_n_aut);
	_n_add	= ntohs(_n_add);

	return 0;
}

/**
 * @method	: DNSQuery::extract_question
 * @return >0	: success, length of question, from header + label + type +
 *	class.
 * @return -1	: fail.
 * @desc	: extract question data from buffer.
 */
int DNSQuery::extract_question()
{
	// start from header length.
	size_t startp = DNS_HDR_SIZE;
	int len = 0;

	// check if buffer size is smaller
	if (startp > _i) {
		return -1;
	}

	// extract question label
	_name.reset();
	len = extract_label(&_name, startp);
	if (len < 0) {
		return -1;
	}
	startp = startp + size_t(len);

	// check 4 byte more for question type and class
	len = int(startp) + 4;
	if (size_t(len) > _i) {
		return -1;
	}

	// extract question type and class
	memcpy(&_q_type, &_v[startp], 2);
	startp += 2;

	memcpy(&_q_class, &_v[startp], 2);

	_q_type		= ntohs(_q_type);
	_q_class	= ntohs(_q_class);

	// set question length, label length + 4 byte
	_q_len		= (uint16_t) (len - DNS_HDR_SIZE);

	return len;
}

/**
 * @method	: DNSQuery::extract_resource_record
 * @desc	: Extract resourse record.
 * @param extract_flag : what type of RR that will be extracted.
 * @return 0	: success.
 * @return -1	: fail, question is empty.
 * @return -2	: fail at extracting RR answer.
 * @return -3	: fail at extracting RR authority.
 * @return -4	: fail at extracting RR additional.
 */
int DNSQuery::extract_resource_record (const char extract_flag)
{
	int	i	= 0;
	size_t len = 0;
	DNS_rr*	rr	= NULL;

	// check if question has been extracted.
	if (_q_len == 0) {
		int s = extract_question ();
		if (s <= 0) {
			return -1;
		}
	}

	if (extract_flag < DNSQ_EXTRACT_RR_ANSWER) {
		// nothing to extract, flag is zero.
		return 0;
	}

	// now that we got _q_len, start extracting from there.
	len		= _q_len + DNS_HDR_SIZE;
	_rr_ans_p	= &_v[len];

	for (i = 0; i < _n_ans; ++i) {
		rr = extract_rr(&len);
		if (!rr) {
			return -2;
		}

		set_max_ttl_from_rr (rr);

		_rr_ans.push_tail(rr);
	}

	if (extract_flag >= DNSQ_EXTRACT_RR_AUTH) {
		_rr_aut_p = &_v[len];
		for (i = 0; i < _n_aut; ++i) {
			rr = extract_rr(&len);
			if (!rr) {
				return -3;
			}

			_rr_aut.push_tail(rr);
		}

		_rr_add_p = &_v[len];
		if (extract_flag >= DNSQ_EXTRACT_RR_ADD) {
			for (i = 0; i < _n_add; ++i) {
				rr = extract_rr(&len);
				if (!rr) {
					return -4;
				}
				_rr_add.push_tail(rr);
			}
		}
	}

	return 0;
}

void DNSQuery::set_max_ttl_from_rr (const DNS_rr* rr)
{
	if (rr->_type != _q_type) {
		return;
	}
	if (rr->_class != _q_class) {
		return;
	}

	if (rr->_ttl > _ans_ttl_max) {
		_ans_ttl_max = rr->_ttl;
	}
}

/**
 * @method		: DNSQuery::extract_rr
 * @param:
 *	> offset	: pointer to position of buffer after RR has been
 *                        extracted.
 *	> type		: type of the last extracted rr.
 * @return:
 *	< DNS_rr*	: success.
 *	< NULL		: fail.
 * @desc		: extract Resource-Record (RR) from buffer 'bfr'.
 */
DNS_rr* DNSQuery::extract_rr(size_t* offset)
{
	int	s	= 0;
	DNS_rr*	rr = new DNS_rr();
	Error err;

	if (!rr) {
		return NULL;
	}

	/* Get RR NAME */
	s = extract_label(&rr->_name, *offset);
	if (s < 0) {
		goto err;
	}
	rr->_name_len	= (uint16_t) s;
	*offset		+= size_t(s);

	/* Check if buffer is wide enough to have rr type */
	s = int(*offset) + 2;
	if (size_t(s) > _i) {
		return rr;
	}

	/* Get RR TYPE (2 bytes) */
	memcpy(&rr->_type, &_v[*offset], 2);
	rr->_type	= ntohs(rr->_type);
	*offset		+= 2;

	/* Check if buffer is wide enough to have rr class */
	s = int(*offset) + 2;
	if (size_t(s) > _i) {
		return rr;
	}

	/* Get RR CLASS (2 bytes) */
	memcpy(&rr->_class, &_v[*offset], 2);
	rr->_class	= ntohs(rr->_class);
	*offset		+= 2;

	/* Check 4 byte TTL */
	s = int(*offset) + 4;
	if (size_t(s) > _i) {
		return rr;
	}

	/* Get RR TTL */
	memcpy(&rr->_ttl, &_v[*offset], 4);
	rr->_ttl	= ntohl(rr->_ttl);
	rr->_ttl_p	= &_v[*offset];
	*offset		+= 4;

	/* Check 2 bytes for DATALEN */
	s = int(*offset) + 2;
	if (size_t(s) > _i) {
		return rr;
	}

	/* Get RR DATALEN */
	memcpy(&rr->_len, &_v[*offset], 2);
	rr->_len	= ntohs(rr->_len);
	*offset		+= 2;

	/* Check if packet length is less equal than DATALEN */
	s = int(*offset + rr->_len);
	if (size_t(s) > _i) {
		return rr;
	}

	/* Check if buffer size is enough, if not resize it DATALEN */
	if (rr->_len > rr->size()) {
		err = rr->resize(rr->_len);
		if (err != NULL) {
			goto err;
		}
	}

	/* Get RR DATA */
	rr->copy_raw(&_v[*offset], rr->_len);

	switch (rr->_type) {
	case QUERY_T_ADDRESS:
		inet_ntop(AF_INET, rr->v(), (char*) rr->_data.v()
			, socklen_t(rr->_data.size()));
		rr->_data.set_len(strlen(rr->_data.v()));
		*offset	+= 4;
		break;

	case QUERY_T_NAMESERVER:
	case QUERY_T_CNAME:
		s = extract_label(&rr->_data, *offset);
		if (s != rr->_len) {
			goto err;
		}
		*offset	+= size_t(s);
		break;

	case QUERY_T_SOA:
		s = extract_label(&rr->_data, *offset);
		if (s < 0) {
			goto err;
		}
		*offset += size_t(s);

		s = extract_label(&rr->_data2, *offset);
		if (s < 0) {
			goto err;
		}
		*offset += size_t(s);

		if ((*offset + 20) > _i) {
			goto err;
		}

		memcpy(&rr->_serial, &_v[*offset], 4);
		rr->_serial	= ntohl(rr->_serial);
		*offset		+= 4;

		memcpy(&rr->_refresh, &_v[*offset], 4);
		rr->_refresh	= ntohl(rr->_refresh);
		*offset		+= 4;

		memcpy(&rr->_retry, &_v[*offset], 4);
		rr->_retry	= ntohl(rr->_retry);
		*offset		+= 4;

		memcpy(&rr->_expire, &_v[*offset], 4);
		rr->_expire	= ntohl(rr->_expire);
		*offset		+= 4;

		memcpy(&rr->_minimum, &_v[*offset], 4);
		rr->_minimum	= ntohl(rr->_minimum);
		*offset		+= 4;
		break;

	case QUERY_T_PTR:
		s = extract_label(&rr->_data, *offset);
		if (s != rr->_len) {
			goto err;
		}
		*offset += size_t(s);
		break;

	case QUERY_T_HINFO:
		s = extract_label(&rr->_data, *offset);
		if (s < 0) {
			goto err;
		}
		*offset += size_t(s);

		s = extract_label(&rr->_data2, *offset);
		if (s < 0) {
			goto err;
		}
		*offset += size_t(s);
		break;

	case QUERY_T_MX:
		memcpy(&rr->_priority, &_v[*offset], 2);
		rr->_priority	= ntohs(rr->_priority);
		*offset		+= 2;

		s = extract_label(&rr->_data, *offset);
		if (s < 0) {
			goto err;
		}
		*offset += size_t(s);
		break;

	case QUERY_T_TXT:
		s = _extract_TXT(rr, *offset);
		if (s != rr->_len) {
			goto err;
		}
		*offset += size_t(s);
		break;

	case QUERY_T_SRV:
		memcpy(&rr->_priority, &_v[*offset], 2);
		rr->_priority	= ntohs(rr->_priority);
		*offset		+= 2;

		memcpy(&rr->_weight, &_v[*offset], 2);
		rr->_weight	= ntohs(rr->_weight);
		*offset		+= 2;

		memcpy(&rr->_port, &_v[*offset], 2);
		rr->_port	= ntohs(rr->_port);
		*offset		+= 2;

		s = extract_label(&rr->_data, *offset);
		if (s < 0) {
			goto err;
		}
		*offset += size_t(s);
		break;

	case QUERY_T_AAAA:
		inet_ntop (AF_INET6, rr->v(), (char*) rr->_data.v()
			, socklen_t(rr->_data.size()));
		rr->_data.set_len(strlen(rr->_data.v()));
		*offset	+= 16;
		break;

	default:
		fprintf(stderr
, "[%s] extract_rr: (%s) Record type '%d' is not handle yet!\n"
		, __cname, _name.chars(), rr->_type);
		goto err;
	}

	return rr;
err:
	if (LIBVOS_DEBUG) {
		fprintf(stderr
	, "[%s] extract_rr: error extracting record type '%d'!"
	, __cname, rr->_type);
	}
	delete rr;
	return NULL;
}

/**
 * @method		: DNSQuery::extract_label
 * @param		:
 *	> label		: return value, content of DNS packet.
 *	> bfr_off	: buffer offset.
 * @return		:
 *	> >=0		: success, length of label.
 *	< -1		: fail.
 * @desc		: read a label from DNS packet buffer.
 */
int DNSQuery::extract_label(Buffer* label, const size_t bfr_off)
{
	int		len	= 0;
	int		ret_len	= 0;
	uint16_t	offset	= 0;
	const char*	p	= (const char*)&_v[bfr_off];

	/* Check buffer offset overflow */
	if (bfr_off > _i) {
		return -1;
	}

	while (*p) {
		/* Bitmask to check for POINTER */
		if ((*p & 0xC0) == 0xC0) {
			/* If we never used pointer before */
			if (!offset) {
				ret_len += 2;
			}

			/* Get POINTER */
			memcpy(&offset, p, 2);
			offset = ntohs(offset);
			offset &= 0x3FFF;
			if (offset > _i) {
				return -1;
			}

			/* Jump to POINTER offset */
			p = (const char*) &_v[offset];

		/* If we never get length before */
		} else if (len == 0) {
			/* Get LENGTH of message (1 byte) */
			len = *p;
			p++;

			/* If this is the last message LENGTH == 0 */
			if (label->len() > 0) {
				label->appendc('.');
			}
			if (!offset) {
				ret_len += len + 1;
			}
		} else {
			/* Pick each character by LENGTH */
			while (*p && len > 0) {
				label->appendc(*p);
				--len;
				p++;
			}
		}
	}
	if (!offset) {
		ret_len++;
	}

	return ret_len;
}

/**
 * `_extract_TXT()` will copy `n` octact of data from answer buffer.
 * The length of TXT record is defined in RDLENGTH of RR.
 */
int DNSQuery::_extract_TXT(DNS_rr* rr, const size_t offset)
{
	const char* p = &_v[offset];
	int len = *p;

	p++;

	rr->_data.copy_raw(p, size_t(len));

	return len + 1;
}

/**
 @method	: DNSQuery::create_answer
 @param		:
 > name		: host name.
 > type		: RR type.
 > clas		: RR class.
 > ttl		: time to live value for record.
 > data_len	: length of data.
 > data		: data.
 > attrs	: additional attribute for dns object.
 @return	:
 < 0		: success.
 < 1		: fail.
 @desc		: Create packet of DNS answer for hostname 'hname' with list of
	address in 'addrs'.
 */
int DNSQuery::create_answer (const char* name
				, uint16_t type, uint16_t clas
				, uint32_t ttl
				, uint16_t data_len, const char* data
				, uint32_t attrs)
{
	uint16_t v = 0;

	reset (DNSQ_DO_ALL);

	_bfr_type	= BUFFER_IS_UDP;
	_name.copy_raw (name);
	_q_type		= type;
	_q_class	= clas;
	_ans_ttl_max	= ttl;
	_attrs		= attrs;

	set_header (0
		, HDR_IS_RESPONSE | OPCODE_QUERY
		, 1, (uint16_t) 1, 0, 0);

	/* Create question section */
	DNS_rr::APPEND_DNS_LABEL (this, name
			, (unsigned int) strlen (name));

	v = htons (_q_type);
	append_bin (&v, 2);

	v = htons (_q_class);
	append_bin (&v, 2);

	/* Create answer section */
	DNS_rr* rr_answer = DNS_rr::INIT (name, type, clas, ttl, data_len
					, data);
	if (!rr_answer) {
		return 1;
	}

	append_raw(rr_answer->v(), rr_answer->len());

	_rr_ans.push_tail(rr_answer);

	return 0;
}

/**
 * @method	: DNSQuery::remove_rr_aut
 * @desc	: remove authority record from buffer.
 *
 *	buffer MUST be extracted before calling this function, using
 *	extract() method.
 */
void DNSQuery::remove_rr_aut()
{
	if (is_empty() || !_rr_aut_p) {
		return;
	}

	if (_n_add > 0 && _rr_add_p) {
		int rr_aut_len = (int)(_rr_add_p - _rr_aut_p);
		int rr_add_len = (int)(&_v[_i] - _rr_add_p);

		memmove((void *)_rr_aut_p, _rr_add_p, size_t(rr_add_len));

		_i = _i - size_t(rr_aut_len);
	} else {
		ssize_t s = _rr_aut_p - _v;
		if (s < 0) {
			_i = 0;
		} else {
			_i = size_t(s);
		}
	}

	_rr_aut.reset();

	_i++;
	_rr_aut_p	= NULL;
	_v[_i]		= 0;
	_n_aut		= 0;
	memset(&_v[DNS_AUT_CNT_POS], 0, 2);
}

/**
 * @method	: DNSQuery::remove_rr_add
 * @desc	: remove additional record from buffer.
 *
 *	buffer MUST be extracted before calling this function, using
 *	extract() method.
 */
void DNSQuery::remove_rr_add()
{
	if (is_empty() || !_rr_add_p) {
		return;
	}

	ssize_t s = 0;

	s = (_rr_add_p - _v);
	if (s < 0) {
		_i = 0;
	} else {
		_i = size_t(s);
	}

	_rr_add.reset();

	_rr_add_p	= NULL;
	_v[_i]		= 0;
	_n_add		= 0;
	memset(&_v[DNS_ADD_CNT_POS], 0, 2);
}

/**
 * @method	: DNSQuery::set_id
 * @param	:
 *	> id	: a new identifier for DNS packet.
 * @desc	: set a new transaction ID for DNS packet buffer.
 */
void DNSQuery::set_id(const int id)
{
	if (is_empty()) {
		return;
	}

	char* p = _v;

	_id = htons((uint16_t) id);

	if (BUFFER_IS_TCP == _bfr_type) {
		p = &_v[2];
	}

	memset(p, 0, 2);
	memcpy(p, &_id, 2);

	_id = (uint16_t) id;
}

/**
 * set_tc will set TrunCated flag on header to `flag` value (0 for not
 * truncated, 1 for truncated).
 */
void DNSQuery::set_tc(const int flag)
{
	if (is_empty()) {
		return;
	}

	char* p = _v;

	if (BUFFER_IS_TCP == _bfr_type) {
		p = &_v[2];
	}

	// Move pointer to flag address.
	p = p + 2;

	// Get current flag.
	memcpy(&_flag, p, 2);
	_flag = ntohs(_flag);

	// Set flag to on or off.
	if (flag == 1) {
		_flag = _flag | RTYPE_TC_ON;
	} else {
		_flag = _flag & RTYPE_TC_OFF;
	}

	// Set flag in header back.
	_flag = htons((uint16_t) flag);
	memcpy(p, &_flag, 2);
	_flag = ntohs(_flag);
}

/**
 * @method	: DNSQuery::set_rr_answer_ttl
 * @param	:
 *	> ttl	: time to live, in seconds. Default to UINT_MAX.
 * @desc	: set TTL value in each RR Answer record to 'ttl'. DNSQuery
 * object must be extracted before calling this function.
 */
void DNSQuery::set_rr_answer_ttl(unsigned int ttl)
{
	DNS_rr* rr	= NULL;
	int	x	= 0;

	ttl = htonl(ttl);

	for (; x < _rr_ans.size(); x++) {
		rr = (DNS_rr*) _rr_ans.at(x);

		if (rr && rr->_ttl_p) {
			memset((void*) rr->_ttl_p, 0, 4);
			memcpy((void*) rr->_ttl_p, &ttl, 4);
		}
	}
}

/**
 * `get_num_answer()` will return number of answer in RR.
 */
int DNSQuery::get_num_answer()
{
	return _rr_ans.size();
}

/**
 * @method		: DNSQuery::reset
 * @param		:
 *	> do_type	: reset type.
 * @desc		: reset DNS packet buffer with reset mode is defined
 *                        by 'do_type'.
 */
void DNSQuery::reset(const int do_type)
{
	_id		= 0;
	_flag		= 0;
	_n_qry		= 0;
	_n_ans		= 0;
	_n_aut		= 0;
	_n_add		= 0;
	_q_type		= 0;
	_q_class	= 0;
	_bfr_type	= BUFFER_IS_UDP;
	_name.reset();

	if (do_type == DNSQ_DO_ALL) {
		Buffer::reset();
	}

	_rr_ans.reset();
	_rr_aut.reset();
	_rr_add.reset();

	_rr_ans_p	= NULL;
	_rr_aut_p	= NULL;
	_rr_add_p	= NULL;
	_ans_ttl_max	= 0;
}

/**
 * @method	: DNSQuery::dump
 * @desc	: print content of DNS packet buffer to standard output.
 */
void DNSQuery::dump(const uint8_t do_type)
{
	printf("\n[%s] dump:\n; Buffer\n", __cname);

	if (_v && do_type == DNSQ_DO_ALL) {
		dump_hex();
	}

	printf("; HEADER section\n");
	printf(" id              : %d\n", _id);
	printf(" flag            : %#4X\n", _flag);
	printf("   query type    : %d\n", _flag & OPCODE_FLAG);
	printf("   response type : %#4X\n", _flag & RTYPE_FLAG);
	printf("   response code : %d\n", _flag & RCODE_FLAG);
	printf(" n question      : %d\n", _n_qry);
	printf(" n answer        : %d\n", _n_ans);
	printf(" n auth          : %d\n", _n_aut);
	printf(" n additional    : %d\n", _n_add);
	printf("; QUESTION section\n");
	printf(" type            : %d\n", _q_type);
	printf(" class           : %d\n", _q_class);
	printf(" name            : %s\n", _name.chars());

	if (_rr_ans.size() > 0) {
		printf("\n; ANSWER section\n%s\n", _rr_ans.chars());
	}
	if (_rr_aut.size() > 0) {
		printf("\n; AUTHENTICATION section\n%s\n", _rr_aut.chars());
	}
	if (_rr_add.size() > 0) {
		printf("\n; ADDITIONAL section\n%s\n", _rr_add.chars());
	}
}

const char* get_type(uint16_t flag)
{
	return _DNS_HDR_TYPE_NAME[(flag >> 15) & 0x0001];
}

const char* get_opcode(uint16_t flag)
{
	return _DNS_HDR_OPCODE_NAME[(flag >> 11) & 0x0003];
}

//
// `get_rcode()` will return DNS packet reply code (RCODE) status.
//
// The status can be,
//
// 0 - No error condition
//
// 1 - Format error - The name server was unable to interpret the query.
//
// 2 - Server failure - The name server was unable to process this query due
// to a problem with the name server.
//
// 3 - Name Error - Meaningful only for responses from an authoritative name
// server, this code signifies that the domain name referenced in the query
// does not exist.
//
// 4 - Not Implemented - The name server does not support the requested kind
// of query.
//
// 5 - Refused - The name server refuses to perform the specified operation
// for policy reasons.  For example, a name server may not wish to provide the
// information to the particular requester, or a name server may not wish to
// perform a particular operation (e.g., zone transfer) for particular data.
//
// 6-15 - Reserved for future use.
//
int DNSQuery::get_rcode()
{
	return _flag & RCODE_FLAG;
}

//
// `is_ok()` is an alias for `get_rcode()`.
//
int DNSQuery::is_ok()
{
	return DNSQuery::get_rcode();
}

//
// `get_rcode_name()` will return string error in DNS reply based on value of
// `_flag`.
// It will return "unknown" if reply code is equal or greater than known reply
// code.
//
// Reference: rfc1035 page 27.
//
const char* DNSQuery::get_rcode_name()
{
	int rcode = get_rcode();

	if (rcode >= RCODE_SIZE) {
		return _DNS_HDR_RCODE_NAME[RCODE_SIZE];
	}

	return _DNS_HDR_RCODE_NAME[rcode];
}

/**
 * `chars()` will return string representation of this object as JSON
 * notation.
 */
const char* DNSQuery::chars()
{
	if (__str) {
		free(__str);
		__str = NULL;
	}

	Buffer b;

	b.append_raw("{\n");

	b.append_fmt("\t" K(ID)     ": %d\n", _id);
	b.append_fmt(",\t" K(TYPE)   ": " K(%s) "\n", get_type(_flag));
	b.append_fmt(",\t" K(OPCODE) ": " K(%s) "\n", get_opcode(_flag));
	b.append_fmt(",\t" K(AA)     ": %d\n", (_flag & RTYPE_AA) ? 1 : 0);
	b.append_fmt(",\t" K(TC)     ": %d\n", (_flag & RTYPE_TC_ON) ? 1 : 0);
	b.append_fmt(",\t" K(RD)     ": %d\n", (_flag & RTYPE_RD) ? 1 : 0);
	b.append_fmt(",\t" K(RA)     ": %d\n", (_flag & RTYPE_RA) ? 1 : 0);
	b.append_fmt(",\t" K(RCODE)  ": " K(%s) "\n", get_rcode_name());

	b.append_raw(",\t" K(ANSWERS) ": ");
	b.append_raw(_rr_ans.chars());
	b.appendc('\n');

	b.append_raw(",\t" K(AUTHORITY) ": ");
	b.append_raw(_rr_aut.chars());
	b.appendc('\n');

	b.append_raw(",\t" K(ADDITIONAL) ": ");
	b.append_raw(_rr_add.chars());
	b.appendc('\n');

	b.append_raw("}");

	__str = b.detach();

	return __str;
}

/**
 * @method	: DNSQuery::INIT
 * @param	:
 *	> o	: return value, pointer to DNSQuery object.
 *	> bfr	: pointer to DNS packet buffer.
 *	> type	: type of DNS packet (UDP or TCP).
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: initialize DNSQuery object 'o' using 'bfr' value.
 */
int DNSQuery::INIT(DNSQuery **o, const Buffer *bfr, const int type)
{
	if (!bfr) {
		return -1;
	}

	(*o) = new DNSQuery();
	if (!(*o)) {
		return -1;
	}

	int s = 0;
	Error err;

	if (type == BUFFER_IS_TCP) {
		s = (*o)->to_udp(bfr);
	} else {
		err = (*o)->copy(bfr);
		if (err != NULL) {
			s = -1;
		}
	}
	if (s != 0) {
		delete (*o);
		(*o) = NULL;
		return -1;
	}

	(*o)->_bfr_type = BUFFER_IS_UDP;

	s = (*o)->extract (DNSQ_EXTRACT_RR_AUTH);
	if (s != 0) {
		delete (*o);
		(*o) = NULL;
		return -2;
	}

	return 0;
}

} /* namespace::vos */
// vi: ts=8 sw=8 tw=78:

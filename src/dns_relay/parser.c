#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void print_resource_record(struct ResourceRecord *rr) {
    int i;
    while (rr) {
        printf("  ResourceRecord { name '%s', type %u, class %u, ttl %u, rd_length %u, ",
               rr->name,
               rr->type,
               rr->cls,
               rr->ttl,
               rr->rd_length);

        union ResourceData *rd = &rr->rd_data;
        switch (rr->type) {
        case A_Resource_RecordType:
            printf("Address Resource Record { address ");

            for (i = 0; i < 4; i += 1)
                printf("%s%u", (i ? "." : ""), rd->a_record.addr[i]);

            printf(" }");
            break;
        case AAAA_Resource_RecordType:
            printf("AAAA Resource Record { address ");
            for (i = 0; i < 16; i += 1)
                printf("%s%02x", (i ? ":" : ""), rd->aaaa_record.addr[i]);
            printf(" }");
            break;
        case TXT_Resource_RecordType:
            printf("Text Resource Record { txt_data '%s' }",
                   rd->txt_record.txt_data);
            break;
        default:
            printf("Unknown Resource Record { ??? }");
        }
        printf("}\n");
        rr = rr->next;
    }
}

void print_message(struct Message *msg) {
    struct Question *q;

    printf("QUERY { ID: %02x", msg->id);
    printf(". FIELDS: [ QR: %u, OpCode: %u ]", msg->qr, msg->opcode);
    printf(", QDcount: %u", msg->qdCount);
    printf(", ANcount: %u", msg->anCount);
    printf(", NScount: %u", msg->nsCount);
    printf(", ARcount: %u,\n", msg->arCount);

    q = msg->questions;
    while (q) {
        printf("  Question { qName '%s', qType %u, qClass %u }\n",
               q->qName,
               q->qType,
               q->qClass);
        q = q->next;
    }

    print_resource_record(msg->answers);
    print_resource_record(msg->authorities);
    print_resource_record(msg->additionals);

    printf("}\n");
}

size_t get8bits(const uint8_t **buffer) {
    uint8_t value;
    memcpy(&value, *buffer, 1);
    *buffer += 1;

    return ntohs(value);
}

size_t get16bits(const uint8_t **buffer) {
    uint16_t value;
    memcpy(&value, *buffer, 2);
    *buffer += 2;

    return ntohs(value);
}

size_t get32bits(const uint8_t **buffer) {
    uint32_t value;
    memcpy(&value, *buffer, 4);
    *buffer += 4;

    return ntohs(value);
}

void put8bits(uint8_t **buffer, uint8_t value) {
    memcpy(*buffer, &value, 1);
    *buffer += 1;
}

void put16bits(uint8_t **buffer, uint16_t value) {
    value = htons(value);
    memcpy(*buffer, &value, 2);
    *buffer += 2;
}

void put32bits(uint8_t **buffer, uint32_t value) {
    value = htonl(value);
    memcpy(*buffer, &value, 4);
    *buffer += 4;
}

char *decode_domain_name(const uint8_t **buffer, size_t offset) {
    char name[256];
    const uint8_t *buf = *buffer;
    int i = 0;
    int j = 0;

    if (buf[0] >= 0xc0) { // 1. pointer type
        int newOffset = (((int)buf[0] & 0x3f) << 8) + buf[1];
        const uint8_t *nameAddr = *buffer - offset + newOffset;
        *buffer += 2;
        return decode_domain_name(&nameAddr, newOffset);
    }

    while (buf[i] != 0 && buf[i] < 0xc0) { // address part
        if (i != 0) {
            name[j] = '.';
            j++;
        }
        int len = buf[i];
        i += 1;

        memcpy(name + j, buf + i, len);
        i += len;
        j += len;
    }
    if (buf[i] == 0x00) { // 2. pure string type
        i++;
    }
    else if (buf[i] >= 0xc0) { // 3. string + pointer
        i++;
        int newOffset = (((int)buf[i - 1] & 0x3f) << 8) + buf[i];
        buf = *buffer - offset + newOffset; // address of pointer
        char *nameRemain = decode_domain_name(&buf, newOffset);
        memcpy(name + j, nameRemain, strlen(nameRemain));
        j += strlen(nameRemain);
        i++;
    }
    else {
        printf("ERROR: decode_domain_name\n");
    }

    *buffer += i;
    name[j] = '\0';
    return strdup(name);
}

void encode_domain_name(uint8_t **buffer, const char *domain) {
    uint8_t *buf = *buffer;
    const char *beg = domain;
    const char *pos;
    int len = 0;
    int i = 0;
    while ((pos = strchr(beg, '.'))) {
        len = pos - beg;
        buf[i] = len;
        i += 1;
        memcpy(buf + i, beg, len);
        i += len;

        beg = pos + 1;
    }

    len = strlen(domain) - (beg - domain);

    buf[i] = len;
    i += 1;

    memcpy(buf + i, beg, len);
    i += len;

    buf[i] = 0;
    i += 1;

    *buffer += i;
}

void decode_header(struct Message *msg, const uint8_t **buffer) {
    msg->id = get16bits(buffer);

    uint32_t fields = get16bits(buffer);
    msg->qr = (fields & QR_MASK) >> 15;
    msg->opcode = (fields & OPCODE_MASK) >> 11;
    msg->aa = (fields & AA_MASK) >> 10;
    msg->tc = (fields & TC_MASK) >> 9;
    msg->rd = (fields & RD_MASK) >> 8;
    msg->ra = (fields & RA_MASK) >> 7;
    msg->rcode = (fields & RCODE_MASK) >> 0;

    msg->qdCount = get16bits(buffer);
    msg->anCount = get16bits(buffer);
    msg->nsCount = get16bits(buffer);
    msg->arCount = get16bits(buffer);
}

void encode_header(struct Message *msg, uint8_t **buffer) {
    put16bits(buffer, msg->id);

    int fields = 0;
    fields |= (msg->qr << 15) & QR_MASK;
    fields |= (msg->rcode << 0) & RCODE_MASK;
    put16bits(buffer, fields);

    put16bits(buffer, msg->qdCount);
    put16bits(buffer, msg->anCount);
    put16bits(buffer, msg->nsCount);
    put16bits(buffer, msg->arCount);
}

int decode_msg(struct Message *msg, const uint8_t *buffer, size_t size) {
    const uint8_t *oriBuffer = buffer;

    decode_header(msg, &buffer);
    
    if (buffer - oriBuffer < 12) return 0;

    // print_hex(buffer, 200);
    // decode Question
    for (uint16_t i = 0; i < msg->qdCount; ++i) {
        struct Question *q = malloc(sizeof(struct Question));
        q->qName = decode_domain_name(&buffer, buffer - oriBuffer);
        q->qType = get16bits(&buffer);
        q->qClass = get16bits(&buffer);

        // 添加到链表前端
        q->next = msg->questions;
        msg->questions = q;
    }

    // decode Answer
    for (uint16_t i = 0; i < msg->anCount; ++i) {
        struct ResourceRecord *rr = malloc(sizeof(struct ResourceRecord));
        decode_resource_records(rr, &buffer, oriBuffer);
        // 添加到链表前端
        rr->next = msg->answers;
        msg->answers = rr;
    }

    // decode Authority
    for (uint16_t i = 0; i < msg->nsCount; ++i) {
        struct ResourceRecord *rr = malloc(sizeof(struct ResourceRecord));
        decode_resource_records(rr, &buffer, oriBuffer);
        // 添加到链表前端
        rr->next = msg->authorities;
        msg->authorities = rr;
    }

    return 1;
}

int decode_resource_records(struct ResourceRecord *rr, const uint8_t **buffer, const uint8_t *oriBuffer) {
    // print_hex(*buffer, 50);
    rr->name = decode_domain_name(buffer, *buffer - oriBuffer);

    rr->type = get16bits(buffer);
    rr->cls = get16bits(buffer);
    rr->ttl = get32bits(buffer);
    rr->rd_length = get16bits(buffer);
    switch (rr->type)
    {
    case A_Resource_RecordType:
        for (int i = 0; i < 4; ++i)
            rr->rd_data.a_record.addr[i] = get8bits(buffer);
        break;
    case AAAA_Resource_RecordType:
        for (int i = 0; i < 16; ++i)
            rr->rd_data.aaaa_record.addr[i] = get8bits(buffer);
        break;
    case TXT_Resource_RecordType:
    {
        rr->rd_data.txt_record.txt_data_len = get8bits(buffer);
        uint8_t txt_len = rr->rd_data.txt_record.txt_data_len;
        char txtData[txt_len + 1];
        for (int i = 0; i < txt_len; ++i)
        {
            printf("%d ", i);
            txtData[i] = get8bits(buffer);
        }
        txtData[txt_len] = '\0';
        rr->rd_data.txt_record.txt_data = strdup(txtData);
        break;
    }
    case CNAME_Resource_RecordType:
        rr->rd_data.cname_record.name = decode_domain_name(buffer, *buffer - oriBuffer);
        break;
    case PTR_Resource_RecordType:
        rr->rd_data.ptr_record.name = decode_domain_name(buffer, *buffer - oriBuffer);
        break;
    case SOA_Resource_RecordType:
        rr->rd_data.soa_record.MName = decode_domain_name(buffer, *buffer - oriBuffer);
        rr->rd_data.soa_record.RName = decode_domain_name(buffer, *buffer - oriBuffer);
        rr->rd_data.soa_record.serial = get32bits(buffer);
        rr->rd_data.soa_record.refresh = get32bits(buffer);
        rr->rd_data.soa_record.retry = get32bits(buffer);
        rr->rd_data.soa_record.expire = get32bits(buffer);
        rr->rd_data.soa_record.minimum = get32bits(buffer);
        break;
    case MX_Resource_RecordType:
        rr->rd_data.mx_record.preference = get16bits(buffer);
        rr->rd_data.mx_record.exchange = decode_domain_name(buffer, *buffer - oriBuffer);
        break;
    case NS_Resource_RecordType:
        rr->rd_data.name_server_record.name = decode_domain_name(buffer, *buffer - oriBuffer);
        break;
    case SRV_Resource_RecordType:
        rr->rd_data.srv_record.priority = get16bits(buffer);
        rr->rd_data.srv_record.weight = get16bits(buffer);
        rr->rd_data.srv_record.port = get16bits(buffer);
        rr->rd_data.srv_record.target = decode_domain_name(buffer, *buffer - oriBuffer);
        break;
    default:
        fprintf(stderr, "ERROR @decode_resource_records: Unknown type %u. => Ignore resource record.\n", rr->type);
        return -1;
    }
    return 0;
}

int encode_resource_records(struct ResourceRecord *rr, uint8_t **buffer) {
    int i;

    while (rr) {
        // Answer questions by attaching resource sections.
        encode_domain_name(buffer, rr->name);
        put16bits(buffer, rr->type);
        put16bits(buffer, rr->cls);
        put32bits(buffer, rr->ttl);
        put16bits(buffer, rr->rd_length);

        switch (rr->type) {
        case A_Resource_RecordType:
            for (i = 0; i < 4; i += 1)
            put8bits(buffer, rr->rd_data.a_record.addr[i]);
            break;
        case AAAA_Resource_RecordType:
            for (i = 0; i < 16; i += 1)
            put8bits(buffer, rr->rd_data.aaaa_record.addr[i]);
            break;
        case TXT_Resource_RecordType:
            put8bits(buffer, rr->rd_data.txt_record.txt_data_len);
            for (i = 0; i < rr->rd_data.txt_record.txt_data_len; i++)
            put8bits(buffer, rr->rd_data.txt_record.txt_data[i]);
            break;
        default:
            fprintf(stderr, "Unknown type %u. => Ignore resource record.\n", rr->type);
            return 0;
        }

        rr = rr->next;
    }

    return 1;
}

int encode_msg(struct Message *msg, uint8_t **buffer) {
    encode_header(msg, buffer);
    struct Question *q = msg->questions;
    while (q) {
        encode_domain_name(buffer, q->qName);
        put16bits(buffer, q->qType);
        put16bits(buffer, q->qClass);
        q = q->next;
    }

    if (!encode_resource_records(msg->answers, buffer)) {
        return 0;
    }

    if (!encode_resource_records(msg->authorities, buffer)) {
        return 0;
    }

    if (!encode_resource_records(msg->additionals, buffer)) {
        return 0;
    }

    return 1;
}
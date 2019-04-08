#include "sample.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <stdio.h>

size_t encode(uint8_t* buffer, size_t data_len)
{
    // Create the message strct
    TopLevelMessage myMessage = TopLevelMessage_init_zero;

    // Populate the fields
    uint8_t thud_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0 };
    memcpy(myMessage.foo.thud.bytes, thud_data, sizeof(thud_data));
    myMessage.foo.thud.size = sizeof(thud_data);
    myMessage.bar = 42;
    myMessage.baz = SampleEnum_ZEROTH;

    // Encode the data into a buffer
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, data_len);
    pb_encode(&stream, TopLevelMessage_fields, &myMessage);
    return stream.bytes_written;
}

void decode(uint8_t* data_buffer, size_t data_len)
{
    TopLevelMessage message = TopLevelMessage_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(data_buffer, data_len);
    pb_decode(&stream, TopLevelMessage_fields, &message);
    printf("bar: %i\n", message.bar);
    printf("baz: %i\n", message.baz);
    printf("foo.thud: ");
    for (int i = 0; i < message.foo.thud.size; i++)
        printf("%i ", message.foo.thud.bytes[i]);
    printf("\n");
}

int main()
{
    uint8_t buffer[128];

    printf("Encoding...\n");
    size_t data_len = encode(buffer, 128);

    printf("Serialized data: ");
    for (int i = 0; i < data_len; i++)
        printf("%i", buffer[i]);

    /* 
     * This is just a sample, but you could transmit the encoded data at this 
     * point if you wanted to.
     * 
     * Alternatively, you could recieve data and then decode it just like this:
    */

    printf("\nDecoding...\n");
    decode(buffer, data_len);

    return 0;
}
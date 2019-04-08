import sample_pb2 as pb


def encode():
    # Create the pb objects
    my_message = pb.TopLevelMessage()

    # Populate the fields
    my_message.foo.thud = bytes([0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0])
    my_message.bar = 42
    my_message.baz = pb.ZEROTH

    # Serialize
    my_message_bytes = my_message.SerializeToString()
    return my_message_bytes


def decode(message_bytes):
    your_message = pb.TopLevelMessage()
    your_message.ParseFromString(message_bytes)
    print(f"bar: {your_message.bar}")
    assert your_message.baz == pb.ZEROTH
    print(f"baz: {your_message.baz}")
    print(f"foo.thud: {list(your_message.foo.thud)}")


if __name__ == "__main__":
    print("Encoding...")
    serialized_bytes = encode()
    print(f"Serialized data: {serialized_bytes}")

    """
    At this point the data is encoded. You could transmit it if you want.

    Alternatively, you could recieve encoded bytes and decode it like this:
    """

    print("Decoding...")
    decode(serialized_bytes)

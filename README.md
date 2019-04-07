# AppNote-Protobufs
An application note demonstrating how protocol buffers can be compiled and used to interface an embedded application with a higher level language.

# Backround
Protocol buffers are a flexible, language agnostic, lightweight method used for serializing structured data. A .proto file is created to descibe the data that will be passed around. The protobuf compiler (protoc) can generate the coded needed to serialize and deserialize the data described by the .proto file. Languages such as C++, C#, Dart, Go, Java, and Python are all officially supported by the protobuf compiler.

For embedded C applications [nanopb](https://github.com/nanopb/nanopb) can be used to generate the needed C source and header files needed to integrate protobufs into a project.

For a complete description of what protobufs are [take a look at this link](https://developers.google.com/protocol-buffers/).

# Requirements

To use protobufs you need a few things:
* A C compiler ([GCC](https://gcc.gnu.org) will be used for this demonstration)
* The protobuf compiler (You can probably use your favorite package manager and install `protobuf`, but if not you can [find and install](https://developers.google.com/protocol-buffers/docs/downloads) a release version, or [build from source](https://github.com/protocolbuffers/protobuf))
* [Nanopb](https://jpa.kapsi.fi/nanopb/) (Note: The Nanopb project is included as a submodule of this project)

* [Python](https://www.python.org) (Or any other language with support for protobufs. We use python in our example.)

# Describing Data
The first step is to decide what data you actually want to send. [Create a .proto](https://developers.google.com/protocol-buffers/docs/overview) file detailing how your data is structured. These files contain a series of messages that are used to structure your data. Each message has one or more data fields in it. Each data field holds a specific [type](https://developers.google.com/protocol-buffers/docs/proto3#scalar) of data. Messages can be nested within eachother allowing for complex data structures to be defined. Aside from scalar types and other message types protobufs also support enumerateions. Each field is given a [unique field number](https://developers.google.com/protocol-buffers/docs/proto3#assigning-field-numbers). This number is used during encodeing and decoding and allows the data definition to evolve while still being backwards compatible.

Here is a simple protobuf file example demonstraiting scalar field types, nested messages and enumerations:
```
/* sample.proto */
syntax = "proto3";

message TopLevelMessage {
  SubMessage foo = 1;
  int32 bar = 2;
  SampleEnum baz = 3;
}

message SubMessage {
  bytes thud = 1;
}

enum SampleEnum {
  ZEROTH = 0;
  FIRST = 1;
  SECOND = 2;
}
```

Options files can be used to supplement a proto file:
```
sample.options
SubMessage.thud max_size:128
```
Make sure the options file is in the same directory as the protofile when compiling.

# Make nanopb
Before we get ahead or ourselves, lets ensure that nanopb is build:
```
make -BC nanopb/generator/proto
```

# Compiling
Once you have described your data, the next step is to generate the code that will implement the serialization and deserialization of your data. To do this run the `protoc` tool that you installed earlier:
```
# Generate python file
protoc --python_out=. sample.proto

# Use nanopb to generate C source and header
protoc --plugin=protoc-gen-nanopb=nanopb/generator/protoc-gen-nanopb  --nanopb_out=. sample.proto
```
You should now have the generated files `sample_pb2.py`, `sample.pb.c`, and `sample.pb.h`.

# Usage
## Python
### Encodeing Data
[Using the compiled files in Python](https://developers.google.com/protocol-buffers/docs/pythontutorial) is simple. First include the generated file in your file:
```
#include sample_pb2.py as pb
```
Next, create an object for the messages you want to use:
```
my_message = pb.TopLevelMessage()
sub_message = pb.SubMessage()
```
Then fill out the fields:
```
sub_message.thud = bytes("Are you suggesting that coconuts migrate?", "utf-8")
my_message.foo = sub_message
my_messge.bar = 42
my_message.baz = pb.ZEROTH
```
Now you can go ahead and serialize your message:
```
my_message_bytes = my_message.SerializeToString()
```

### Decoding Data
To decode data, simply do the opposite of encodeing. First create an object and deserialize the message:
```
your_message = pb.TopLevelMessage()
your_message.ParseFromString(my_message_bytes)
```
Next access the fields of the message:
```
print(f"Thud: {your_message.foo.thud})
print(f"Bar: {yourmessage.bar}")
assert your_message.baz == pb.ZEROTH
```

## C
### Encodeing Data
In order to utilize the generated C source and headers, you need to include the nanopb encode and decode headers:
```
#include <pb_decode.h>
#include <pb_encode.h>
```
You will also need to include the generated header file:
```
#include "can-d.pb.h"
```
The generated files use structs to match the structure of your data. To populate one of these structs, first init the struct to zeros or to default values using one of the macros that was generated for you:
```
TopLevelMessage myMessage = TopLevelMessage_init_zero;
SubMessage subMessage = SubMessage_init_zero;
```
Then fill in its fields:
```
char* thud_data = "This is not a test.";
memcpy(subMessage.contents.thud.bytes, thud_data, sizeof(thud_data));
myMessage.foo = subMessage;
myMessage.bar = 7;
myMessage.baz = SampleEnum_ZEROTH;
```
Next, create an output stream and encode:
```
uint8_t buffer[128];
pb_ostream_t stream = pb_ostream_from_buffer(buffer, 128);
pb_encode(&stream, SampleMessage_fields, msg);
```
The buffer is now full with the encoded data.

### Decoding Data
Create a buffer initialize a message, create a input stream, and decode.
```
SampleMessage message = SampleMessage_init_zero;
pb_istream_t stream = pb_istream_from_buffer(buffer, 128);
pb_decode(&stream, SampleMessage_fields, &message);
```
You can now access the fields of the message struct.

# Remarks
This short note is only intended to get you started. There are many other features of protobufs that were not touched on: default values, "one of" fields, deserialization to JSON, and much more. Here are some resources to round out your undertanding of protobufs:
* [Protobuf documentation](https://developers.google.com/protocol-buffers/) for details about protobufs in general
* [Nanopb documentation](https://jpa.kapsi.fi/nanopb/) for details about Nanopb
* [Nanopb benchmark](https://github.com/PetteriAimonen/nanopb-benchmark) With some interesting makefiles
* [Protobuf C](https://github.com/protobuf-c/protobuf-c) An alternative to Nanopb

import argparse
import jinja2
import ProtoParser

def main():

    arg_parser = argparse.ArgumentParser(description = 'PacketGenerator')
    arg_parser.add_argument('--path', type=str, default='./Protocol.proto', help='proto path')
    arg_parser.add_argument('--output', type=str, default='TestPacketHandler', help='output file')
    arg_parser.add_argument('--output2', type=str, default='Packet', help='output file')
    arg_parser.add_argument('--output3', type=str, default='TestPacketHandler2', help='output file')
    arg_parser.add_argument('--recv', type=str, default='CL_', help='recv convention')
    arg_parser.add_argument('--send', type=str, default='LC_', help='send convention')
    args = arg_parser.parse_args()

    parser = ProtoParser.ProtoParser(1000, args.recv, args.send)
    parser.parse_proto(args.path)

    # jinja2
    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)

    template = env.get_template('PacketHandler.h')
    output = template.render(parser = parser, output=args.output)

    template2 = env.get_template('Packet.h')
    output2 = template2.render(parser = parser, output=args.output, output3=args.output3)

    template3 = env.get_template('Packet.cpp')
    output3 = template3.render()

    f = open(args.output+'.h', 'w+')
    f.write(output)
    f.close()

    f = open(args.output2+'.h', 'w+')
    f.write(output2)
    f.close()

    f = open(args.output2+'.cpp', 'w+')
    f.write(output3)
    f.close()

    return

if __name__ == '__main__':
    main()
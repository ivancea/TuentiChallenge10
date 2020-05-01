using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text.RegularExpressions;

namespace _3
{
    enum CommandType {
        ACCEPT,
        ACCEPTED,
        ACCEPT_IGNORE,
        LEARN,
        PREPARE,
        PROMISE,
        PROMISE_DENIAL
    }

    class Command {
        private static readonly Regex CommandRegex =
            new Regex(@"^ROUND (?<round>\d+): (?:(?<sender>\d+) -> )?(?<command>ACCEPT|ACCEPTED|ACCEPT_IGNORE|LEARN|PREPARE|PROMISE|PROMISE_DENIAL) (?<body>.*?)(?: \((?<comments>[^)]*)\))?(?: -> (?<receiver>\d+))?$");
        private static readonly Regex IdRegex = new Regex(@"{(?<a>\d+),(?<b>\d+)}");
        private static readonly Regex ServersRegex = new Regex(@"\[(?<servers>\d+(?:,\d+)*)]");
        private static readonly Regex SecretOwnerRegex = new Regex(@"secret_owner: (?<secretOwner>\d+)");
        private static readonly Regex PromiseTextRegex = new Regex(@"{\d+,\d+}(?<promiseText>.*)");

        public readonly int round;
        public readonly int? sender;
        public readonly int? receiver;
        public readonly CommandType command;
        public readonly string body;
        public readonly string comments;

        public readonly int? idA;
        public readonly int? idB;
        public readonly ISet<int> servers;
        public readonly int? secretOwner;
        public readonly string promiseText;

        private int? GetIntOrDefault(Match match, string groupName, int? defaultValue) {
            var group = match.Groups[groupName];

            if (!group.Success) {
                return defaultValue;
            }

            return int.Parse(group.Value);
        }

        private string GetOrDefault(Match match, string groupName, string defaultValue) {
            var group = match.Groups[groupName];

            if (!group.Success) {
                return defaultValue;
            }

            return group.Value;
        }

        public Command(string line) {
            Match match = CommandRegex.Match(line);

            if (!match.Success) {
                throw new Exception("Can't parse line");
            }

            round = GetIntOrDefault(match, "round", 0) ?? 0;
            sender = GetIntOrDefault(match, "sender", null);
            receiver = GetIntOrDefault(match, "receiver", null);
            command = (CommandType) Enum.Parse(typeof(CommandType), match.Groups["command"].Value);
            body = GetOrDefault(match, "body", null);
            comments = GetOrDefault(match, "comments", null);

            {
                Match idMatch = IdRegex.Match(body);

                if (idMatch.Success) {
                    idA = GetIntOrDefault(idMatch, "a", null);
                    idB = GetIntOrDefault(idMatch, "b", null);
                }
            }

            {
                Match serversMatch = ServersRegex.Match(body);

                if (serversMatch.Success) {
                    servers = GetOrDefault(serversMatch, "servers", null)?.Split(",")
                        .Where(s => s.Length > 0)
                        .Select(int.Parse)
                        .ToHashSet();
                }
            }

            {
                Match secretOwnerMatch = SecretOwnerRegex.Match(body);

                if (secretOwnerMatch.Success) {
                    secretOwner = GetIntOrDefault(secretOwnerMatch, "secretOwner", null);
                }
            }

            if (command == CommandType.PROMISE) {
                Match promiseTextMatch = PromiseTextRegex.Match(body);

                if (promiseTextMatch.Success) {
                    promiseText = GetOrDefault(promiseTextMatch, "promiseText", null);
                }
            }
        }

        public override string ToString() {
            string commentsString = "";
            
            if (comments != null) {
                commentsString = $" ({comments})";
            }

            return $"[{round}] {sender} -> {receiver} : {command} {body}{commentsString}";
        }
    }

    public class Program
    {
        private static void SendPrepare(StreamWriter writer, int idA, int target) {
            writer.WriteLine($"PREPARE {{{idA},9}} -> {target}");
            writer.Flush();
        }
        
        private static void SendAccept(StreamWriter writer, int idA, int idB, IEnumerable<int> servers, int secretOwner, int target) {
            writer.WriteLine($"ACCEPT {{id: {{{idA},{idB}}}, value: {{servers: [{string.Join(",", servers)}], secret_owner: {secretOwner}}}}} -> {target}");
            writer.Flush();
        }

        public static void Main(string[] args){
            TcpClient client = new TcpClient("52.49.91.111", 2092);
            Dictionary<int, TcpClient> dummyClients = new Dictionary<int, TcpClient>();

            StreamReader reader = new StreamReader(client.GetStream());
            StreamWriter writer = new StreamWriter(client.GetStream());

            int serverId = int.Parse(reader.ReadLine().Substring("SERVER ID: ".Length));

            Console.WriteLine($"SERVER ID: {serverId}");

            while(reader.ReadLine() != "AUTOJOIN CLUSTER ENABLED");

            int currentId = 1;

            int lastSecretOwner = 1;
            ISet<int> allServers = new HashSet<int>();
            ISet<int> promisedServers = new HashSet<int>();
            bool waitingForLearn = true;

            while (client.Connected) {
                string line = reader.ReadLine();

                if (line == null) {
                    break;
                }

                try {
                    Command command = new Command(line);
                    
                    if (command.sender != null) {
                        if (command.command == CommandType.LEARN) {
                            allServers = command.servers ?? allServers;
                            lastSecretOwner = command.secretOwner ?? lastSecretOwner;

                            currentId++;
                            promisedServers.Clear();
                            waitingForLearn = false;
                            
                            foreach (int server in allServers) {
                                SendPrepare(writer, currentId, server);
                            }
                        } else if (command.command == CommandType.PROMISE) {
                            if (command.idA == currentId && allServers.Contains(command.sender ?? 9)) {
                                promisedServers.Add(command.sender.Value);

                                if (!waitingForLearn && promisedServers.Count - 1 > allServers.Count / 2) {
                                    waitingForLearn = true;

                                    if (allServers.Where(n => dummyClients.ContainsKey(n)).Count() > allServers.Count/2) {
                                        for(int i=0; i<20; i++)Console.WriteLine("OK");
                                        foreach (int server in promisedServers) {
                                            SendAccept(writer, currentId, 9, allServers, 9, server);
                                        }
                                    } else if (allServers.Count <= 6) {
                                        if (dummyClients.Count == 0) {
                                            for (int i=0; i<4; i++) {
                                                TcpClient dummyClient = new TcpClient("52.49.91.111", 2092);
                                                StreamReader dummyReader = new StreamReader(dummyClient.GetStream());
                                                StreamWriter dummyWriter = new StreamWriter(dummyClient.GetStream());
                                                int dummyClientId = int.Parse(dummyReader.ReadLine().Substring("SERVER ID: ".Length));
                                                dummyClients[dummyClientId] = dummyClient;
                                                Console.WriteLine($"Dummy client: {dummyClientId}");
                                            }
                                        }

                                        int dummyClientToAdd = dummyClients.Keys
                                            .Where(n => !allServers.Contains(n))
                                            .First();

                                        Console.WriteLine($"Attempting to add {dummyClientToAdd}");

                                        foreach (int server in promisedServers) {
                                            SendAccept(writer, currentId, 9, allServers.Append(dummyClientToAdd), lastSecretOwner, server);
                                        }
                                    } else {
                                        int excludedServer = allServers
                                            .Where(n => n != lastSecretOwner && n != 9 && !dummyClients.ContainsKey(n))
                                            .First();
                                        var nextServers = allServers.Where(n => n != excludedServer);
                                        foreach (int server in promisedServers) {
                                            SendAccept(writer, currentId, 9, nextServers, lastSecretOwner, server);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (command.sender != null && command.command != CommandType.PROMISE) {
                        Console.WriteLine(command);
                    }
                } catch (Exception exc) {
                    Console.WriteLine($"Error: {exc.Message}");
                    Console.WriteLine($"- On line: {line}");
                }
            }
        }
    }
}

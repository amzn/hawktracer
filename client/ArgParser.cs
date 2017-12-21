using System;
using System.Collections.Generic;

namespace hawktracerclient
{
    public class ArgParser
    {
        private class Option
        {
            public string Help { get; set; }

            public Type Type { get; set; }
        }

        private string[] arguments;
        private readonly string helpOption;
        private readonly string optionPrefix;
        readonly Dictionary<string, object> values = new Dictionary<string, object>();
        readonly Dictionary<string, Option> options = new Dictionary<string, Option>();

        readonly ISet<string> freeValues = new HashSet<string>();

        public ArgParser(string helpOption, string optionPrefix, string[] args)
        {
            arguments = args;
            this.helpOption = helpOption;
            this.optionPrefix = optionPrefix;

            RegisterOption(helpOption, typeof(bool), "Print this help");
        }

        public void PrintHelp()
        {
            Console.WriteLine("Command line options:");

            foreach (var option in options)
            {
                Console.WriteLine("   " + optionPrefix + option.Key + 
                    (option.Value.Type == typeof(bool) ? "" : "=<value>") + 
                    " - " + option.Value.Help);
            }
        }

        public void Parse()
        {
            foreach (var arg in arguments)
            {
                if (arg.StartsWith(optionPrefix))
                {
                    LoadNamedOption(arg.Substring(2));
                }
                else
                {
                    freeValues.Add(arg);
                }
            }
        }

        public void RegisterOption(string option, Type type, string help)
        {
            options.Add(option, new Option{ Help = help, Type = type });
        }

        public bool HasOption(string option)
        {
            return values.ContainsKey(option);
        }

        public T Get<T>(string option)
        {
            if (!options.ContainsKey(option))
            {
                throw new Exception("Option '" + option + "' has not been registered");
            }

            Type type = options[option].Type;
            if (typeof(T) != type)
            {
                throw new Exception("Expected type '" + typeof(T) + "' but registered '" + type + "'");
            }

            return (T) values[option];
        }

        public ISet<string> GetFreeValues() // TODO should be readonly
        {
            return freeValues;
        }

        private void LoadNamedOption(string arg)
        {
            int eqPos = arg.IndexOf('=');
            string option;
            string value = "";
            if (eqPos == -1)
            {
                option = arg;
            }
            else
            {
                option = arg.Substring(0, eqPos);
                value = arg.Substring(eqPos + 1);
            }

            if (options.ContainsKey(option))
            {
                Type type = options[option].Type;
                if (String.IsNullOrEmpty(value) && type == typeof(bool))
                {
                    values.Add(option, true);
                }
                else
                {
                    values.Add(option, Convert.ChangeType(value, type));
                }
            }
            else
            {
                throw new Exception("invalid option: '" + option + "'. Try " + optionPrefix + helpOption);
            }
        }
    }
}


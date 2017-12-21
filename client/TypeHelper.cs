using System;

namespace HawkTracer.Client
{
    public static class TypeHelper
    {
        public static bool IsNumericType(Type type)
        {
            switch (Type.GetTypeCode(type))
            {
                case TypeCode.Byte:
                case TypeCode.SByte:
                case TypeCode.UInt16:
                case TypeCode.UInt32:
                case TypeCode.UInt64:
                case TypeCode.Int16:
                case TypeCode.Int32:
                case TypeCode.Int64:
                    return true;
                default:
                    return false;
            }
        }

        public static Type GetType(DataType type, UInt64 size)
        {
            switch (type)
            {
                case DataType.Structure:
                    return typeof(Event);
                case DataType.Integer:
                    return GetIntTypeBySize(size);
                case DataType.Pointer:
                    return typeof(IntPtr);
                case DataType.String:
                    return typeof(string);
                default:
                    return null;
            }
        }

        private static Type GetIntTypeBySize(ulong size)
        {
            switch (size)
            {
                case 1:
                    return typeof(byte);
                case 2:
                    return typeof(UInt16);
                case 4:
                    return typeof(UInt32);
                case 8:
                    return typeof(UInt64);
                default:
                    return null;
            }
        }
    }
}


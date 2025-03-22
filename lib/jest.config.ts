import type { Config } from '@jest/types'

const config: Config.InitialOptions = {
  preset: 'ts-jest',
  testEnvironment: 'node',
  globals: {
    'ts-jest': {
      tsconfig: 'tsconfig.test.json',
    },
  },
  coveragePathIgnorePatterns: ['/build/', '/node_modules/', '/__tests__/', 'tests'],
  coverageDirectory: '<rootDir>/coverage/',
  testMatch: ['**/?(*.)+(spec|test).[tj]s?(x)'],
  transform: {
    '^.+\\.tsx?$': [
      'ts-jest',
      {
        isolatedModules: true,
      },
    ],
  },
  roots: ['<rootDir>'],
}

export default config
